import { Injectable } from '@angular/core';
import {$WebSocket} from "angular2-websocket/angular2-websocket";
import {Subject} from "rxjs/Subject";
import {Observable} from "rxjs/Observable";

@Injectable()
export class RpiService {
  private max_data = 100;

  public reflexion_data:Measure = new Measure();
  public surface_data:Measure = new Measure();
  public object_temp_data:Measure = new Measure();
  public mirror_data:Measure = new Measure();
  public aw_data:Measure = new Measure();

  public devMode = false;
  public connectionError = false;
  public ConnectionStatusObservable:Subject<boolean>;
  public ws = null;
  public rpiData:Rpiauto = {
    time: 0,
    time_measure: 0,
    reflexion: 0,
    detection: 0,
    t_surface: 0,
    t_mirror: 0,
    t_object: 0,
    peltier: 0,
    aw_inst: 0,
    aw_medium: 0,
    aw_finale: 0,
    aw_int: 0,
    aw_flash: 0,
    fan: 0,
    clean_mirror: 0,
    measure_started: 0,
    current_task: 0,
    button_magnetic: 0,
    button_ok: 0,
    button_menu: 0,
    xi: 0,
    reflexion_mV: 0,
    adc0 :0,
    adc1 :0,
    adc2 :0,
    adc3 :0,
    adc4 :0,
    adc5 :0,
    adc6 :0,
    adc7 :0,
  };
  public currentAw = NaN;

  private Events = new Subject();
  private UIEvents = new Subject();
  private DataEvent = new Subject();

  //for promise rx
  private req_id = 0;
  private subjectMap:Map<number,Subject<any>> = new Map<number,Subject<any>>();

  private devModeTimer = -1;
  private buttonTimer = 0;

  private ip:string;
  private connectionTimer;
  public buttons = new Button(this,this.Events);

  constructor() {
    this.ConnectionStatusObservable = new Subject();
  }

  public init(ip:string){
    if (this.ws != null){
        this.ws.close(true); //close immediately
    }

    this.ws = new $WebSocket("ws://"+ip,["app"]);

    this.ip = ip;

    this.ws.onMessage(
      (msg: MessageEvent)=> {
        const data = JSON.parse(msg.data);
        if (data.type === "auto") {
          this.rpiData = data;

          //get right aw
          if (this.rpiData.measure_started === 1) {
              this.currentAw = this.rpiData.aw_inst;
          }else
              this.currentAw = this.rpiData.aw_finale;

          //check for buttons
          this.buttons.execute();
          if (this.rpiData.button_menu === 0 && this.rpiData.button_ok === 0){
              if (this.devModeTimer === -1){
                  this.devModeTimer = Date.now();
              }else
                if ((Date.now() - this.devModeTimer) > 3000){
                    this.cmd("buzzer",500);
                    this.devModeTimer = -1;
                    this.devMode = !this.devMode;
              }
          }else {
            this.devModeTimer = -1;
          }

          if (this.connectionError === true){
            this.ConnectionStatusObservable.next(false);
          }
          this.connectionError = false;



          /*reconnect ability*/
          if (this.connectionTimer != null){
            clearTimeout(this.connectionTimer);
          }

          //manage connection lost
          this.connectionTimer = setTimeout(()=>{
            if (this.connectionError === false)
              this.ConnectionStatusObservable.next(true);
            this.connectionError = true;
            this.init(this.ip);
            console.error("connection error, retry connection");

            /*enable reconnect*/
            this.connectionTimer = setInterval(()=>{
              console.error("connection error, retry connection");
              this.init(this.ip);
            },3000);

          },3000);


        }else
        if (data.type === "event") {
          this.Events.next(data.event);
          console.log("EVENT: [%s]",data.event);
        }else
        if (data.type === "dialog") {
          this.UIEvents.next(data);
          console.log("DIALOG: [%s][%s]",data.dialog,data.message);
        }else
        if (data.type === "cmd") {
          //response cmd
          if (data.cmd.indexOf("config.") === 0){
              if (data.response == null){
                 //ok,error
                 const sub = this.subjectMap.get(data.id);
                 sub.next(data.result === "ok");
                 sub.complete();
                 this.subjectMap.delete(data.id);
              }else{
                //number value
                const sub = this.subjectMap.get(data.id);
                sub.next(data.response);
                sub.complete();
                this.subjectMap.delete(data.id);
              }
          }else{
            //feed all json
            const sub = this.subjectMap.get(data.id);
            sub.next(data);
            sub.complete();
            this.subjectMap.delete(data.id);
          }
        }
      },
      {autoApply: false}
    );

    //start task for graph
    setInterval(()=>{
      this.reflexion_data.push(this.rpiData.reflexion);
      this.surface_data.push(this.rpiData.t_surface/*25+Math.random()*1*/);
      this.mirror_data.push(this.rpiData.t_mirror);
      this.object_temp_data.push(this.rpiData.t_object);

      if (this.rpiData.measure_started) {
        this.aw_data.push(this.rpiData.aw_inst);
      }

      this.DataEvent.next();

    },1000);
  }

  private sendCmd(obj:any):Subject<any>{
    const id = this.req_id++;
    const subject = new Subject();
    this.subjectMap.set(id,subject);
    obj.id = id;
    this.ws.send4Direct(obj);
    return subject;
  }

  public cmd(name:string,value:any = null):Subject<RpiCmd>{
    return this.sendCmd({cmd:name,value:value});
  }

  public writeConfig(name:string,value:any):Subject<boolean>{
    return this.sendCmd({cmd:'config.'+name,value:value});
  }

  public readConfig(name:string):Subject<any>{
    return this.sendCmd({cmd:'config.'+name});
  }

  public subjectEvents():Subject<any>{
    return this.Events;
  }

  public subjectUi():Subject<any>{
    return this.UIEvents;
  }

  public subjectDataEvent():Subject<any>{
    return this.DataEvent;
  }

  public resetMeasure(){
    this.reflexion_data.clear();
    this.surface_data.clear();
    this.mirror_data.clear();
    this.aw_data.clear();
  }
}

export class Button{
  private buttonTimer = 0;
  private button = "";

  constructor(private rpi:RpiService,private Events:Subject<any>){
  }

  public press(button:string){
      switch (button){

        case "button_ok":
          this.rpi.rpiData.button_ok = 0;
          break;

        case "button_menu":
          this.rpi.rpiData.button_menu = 0;
          break;

          case "button_all":
            this.rpi.rpiData.button_menu = 0;
            this.rpi.rpiData.button_ok = 0;
        break;
      }

    this.Events.next(button);
    console.log("Event: [%s]",this.button);
  }

  public execute(){
    if (this.rpi.rpiData.button_ok === 0 && this.rpi.rpiData.button_menu === 0){
      if (this.button !== "button_all"){
        this.FeedValue(1);
      }
      this.button = "button_all";
      this.FeedValue(this.rpi.rpiData.button_ok || this.rpi.rpiData.button_menu);
    }else
    if (this.rpi.rpiData.button_ok === 0){
      if (this.button !== "button_ok"){
        this.FeedValue(1);
      }
      this.button = "button_ok";
      this.FeedValue(this.rpi.rpiData.button_ok);
    }else
    if (this.rpi.rpiData.button_menu === 0){
      if (this.button !== "button_menu"){
        this.FeedValue(1);
      }
      this.button = "button_menu";
      this.FeedValue(this.rpi.rpiData.button_menu);
    }else
      this.FeedValue(1);
  }

  public FeedValue(status:number){
    if (status === 0 && this.buttonTimer === 0) {
      this.buttonTimer = Date.now();
    }else
    if (this.buttonTimer !== -1 && status === 0 && (Date.now() - this.buttonTimer) >= 60){
      //buzzer
      this.rpi.cmd(RpiCommands.buzzer,300).subscribe(()=>{});
      this.buttonTimer = -1;
      this.Events.next(this.button);
      console.log("Event: [%s]",this.button);
    }else
    if (status === 1){
      this.buttonTimer = 0;
    }
  }
}

export class Measure{
  public dataList: number[] = [];
  public min:number = NaN;
  public max:number = NaN;

  private sum:number = 0;
  private value_count:number = 0;

  constructor(private max_data = 100){
  }

  public push(value:number){
    //avg
    this.value_count++;
    this.sum += value;

    //min
    if (this.min > value || isNaN(this.min)) this.min = value;
    //max
    if (this.max < value || isNaN(this.max)) this.max = value;

    this.dataList.push(value);

    if (this.dataList.length > this.max_data)
      this.dataList.shift();
  }

  public avg():number{
    return this.sum / this.value_count;
  }

  public clear(){
   this.dataList = [];
    this.min = NaN;
    this.max = NaN;
    this.sum = 0;
    this.value_count = 0;
  }
}

export let RpiCommands = {
  dialog:"dialog",
  history:"history",
  peltier:"peltier",
  fan:"fan",
  buzzer:"buzzer",
  measure:"measure",
  aw:"aw",
  testaw:"testaw",
  kinetics:"kinetics"
};

export let RpiConfig = {
  //misc
   LANGUAGE: "LANGUAGE",
   BIP_ON_BUTTON: "BIP_ON_BUTTON" ,
   BUZZ_FREQ: "BUZZ_FREQ",
   LCD_REFRESH: "LCD_REFRESH",
   LCD_ID: "LCD_ID",

  //hardware
   FAN_SPEED:"FAN_SPEED", //0 - 100

  //DELTA
   DELTA_TEMP_SURF:"DELTA_TEMP_SURF",
   DELTA_TEMP_MIRROR:"DELTA_TEMP_MIRROR",
   DELTA_VALUE_IR:"DELTA_VALUE_IR",

  //slide
   SLOPE_TEMP_SURF:"SLOPE_TEMP_SURF",
   SLOPE_TEMP_MIRROR:"SLOPE_TEMP_MIRROR",
   SLOPE_VALUE_IR: "SLOPE_VALUE_IR",

  //config
   MEASURE_MODE : "MEASURE_MODE", //enum MEASURE_MODE
   KINETIC_MODE : "KINETIC_MODE", //enum KINETIC_MODE
   CALIBRATION_MODE : "CALIBRATION_MODE", //enum CALIBRATION_MODE
   AUTOSTART:"AUTOSTART", //enum AUTOSTART
   COMPARATION_MODE:"COMPARATION_MODE",

  AVERAGE_TMIR: "AVERAGE_TMIR",
  FREQUENCY_TMIR: "FREQUENCY_TMIR",

   UI_STORAGE:"UI_STORAGE",
   UI_MODE:"UI_MODE",

   DELTA_AW: "DELTA_AW",
   SLOPE_AW: "SLOPE_AW",

   SAMPLE_TEMP: "SAMPLE_TEMP",

   SEUIL: "SEUIL",
    MIRROR_AUTO_SHIFT: "MIRROR_AUTO_SHIFT"
  //comparation

};

export enum Rpi_MEASURE_MODE {
  _fix_10_m,
  _2_5_flash,
  _3_10_flash,
  _6_30_flash,
  _10_flash,
  _2_5m,
  _3_10m,
  _6_30m
};
export let Rpi_MEASURE_MODE_list = [0,1,2,3,4,5,6,7];
export let Rpi_MEASURE_MODE_str = [
  "2_5m",
  "3_10m",
  "6_30m",
  "fix_10_m",
  "2_5_flash",
  "3_10_flash",
  "6_30_flash",
  "10_flash",
];

export enum Rpi_KINETIC_MODE {
  _15m_2_30,
  _30m_2_30,
  _1h_15,
  _2h_10,
  _4h_15,
  _8h_30,
  _cont_60,
  _cont_2_30,
};
export let Rpi_KINETIC_MODE_list = [0,1,2,3,4,5,6,7];
export let Rpi_KINETIC_MODE_str = [
  "15m_2_30",
  "30m_2_30",
  "1h_15",
  "2h_10",
  "4h_15",
  "8h_30",
  "cont_60",
  "cont_2_30",
];

export enum Rpi_CALIBRATION_MODE {
  _1p_1l,
  _1p_2l,
  _1p_3l,
  _1p_4l,
  _1p_5l,
  _1p_6l,

  _2p_1l = 128,
  _2p_2l,
  _2p_3l,
  _2p_4l,
  _2p_5l,
  _2p_6l,
};
export let Rpi_CALIBRATION_MODE_list = [0,1,2,3,4,5,128,129,130,131,132,133];
export let Rpi_CALIBRATION_MODE_str = [
  "1p_1l",
  "1p_2l",
  "1p_3l",
  "1p_4l",
  "1p_5l",
  "1p_6l",
  "2p_1l",
  "2p_2l",
  "2p_3l",
  "2p_4l",
  "2p_5l",
  "2p_6l",
];

export enum Rpi_AUTOSTART {
  off,
  measure_1,
  measure_2,
  kinetic_1,
  kinetic_2,
};
export let Rpi_AUTOSTART_list = [0,1,2,3,4];
export let Rpi_AUTOSTART_str = [
  "off",
  "measure_1",
  "measure_2",
  "kinetic_1",
  "kinetic_2"];
export enum Rpi_COMPARATION_MODE {
  greater,
  lower,
}
export let Rpi_COMPARATION_MODE_list = [0,1];
export let Rpi_COMPARATION_MODE_str = [
  ">",
  "<",
];

export interface RpiCmd{
  id:number,
  cmd:string,
  result:string,
  response:any,
}



export enum RpiUiEvents {
  ui_magnetic_down, //put magnetic down
  ui_salt_1, //put salt 1
  ui_salt_2, //put salt 2
  ui_clean_mirror, //clean mirror
  ui_aborted_magnetic, //magnetic is up, op aborted
}

export interface RpiUiEvt{
  dialog:string, //
  message:RpiUiEvents
}

export let RpiEvents = {
  aw : "aw",
  measure_end : "measure_end",
  measure_error : "measure_error",
  measure_aborted : "measure_aborted",
  calibration_error : "calibration_error",
  calibration_error_salt:"calibration_error_salt", //same salt found
  calibration_error_close_salt:"calibration_error_close", //
  calibration_end:"calibration_end",
  calibration_started:"calibration",

  //virtual
  button_ok : "button_ok",
  button_menu : "button_menu",
  button_all: "button_all"
};

export let RpiUiDialog = {
  info : "info",
  error : "error",
  input : "input"
};

export interface RpiMeasureData{
  aw:number,
  date:number,
  t_surface:number,
}

export interface Rpiauto{
  time: number,
  time_measure: number,
  reflexion: number,
  detection: number,
  t_surface: number,
  t_mirror: number,
  t_object: number,
  peltier: number,
  aw_inst: number,
  aw_medium: number,
  aw_finale: number,
  aw_int: number,
  aw_flash: number,
  xi: number,
  reflexion_mV: number,
  fan: number,
  clean_mirror: number,
  measure_started: number,
  current_task: number,
  button_magnetic: number,
  button_ok: number,
  button_menu: number,
  //adc
  adc0 :number,
  adc1 :number,
  adc2 :number,
  adc3 :number,
  adc4 :number,
  adc5 :number,
  adc6 :number,
  adc7 :number,
}
