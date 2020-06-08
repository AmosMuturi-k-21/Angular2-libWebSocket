import {Component, OnInit, ViewChild, ElementRef, OnDestroy, DoCheck, AfterViewInit, QueryList, ViewChildren} from '@angular/core';
import {RpiCmd, RpiCommands, RpiEvents, RpiService, RpiUiDialog, RpiUiEvents, RpiUiEvt} from "../rpi.service";
import {Menu} from "../menu";
import {Subscription} from "rxjs/Subscription";
import {LenguageService} from "../lenguage.service";
import {StorageService} from "../storage.service";

@Component({
  selector: 'app-calibration-menu',
  templateUrl: './calibration-menu.component.html',
  styleUrls: ['./calibration-menu.component.css']
})
export class CalibrationMenuComponent implements OnInit, OnDestroy,AfterViewInit{

  public menu:Menu = new Menu("calibration");
  @ViewChild('b11') b1:ElementRef;
  @ViewChild('b22') b2:ElementRef;

  /*@ViewChild('b3') b3:ElementRef;
  @ViewChild('b4') b4:ElementRef;*/

  alertMsg:string;
  alert:boolean;
  public alertBtn: boolean = false;
  alertSalt:boolean = false;
  public calibrationText = "";
  public measureButton = this.lang.Language.start;
  measureStartedYet:boolean;
  calibStartedYet:boolean;


  calibStage = 0;

  constructor(public rpi: RpiService,public lang:LenguageService,public store:StorageService ) {
  }

  private events:Subscription = null;
  private Dataevents:Subscription = null;
  private Uievents:Subscription = null;

  ngOnDestroy(){
    if (this.events) this.events.unsubscribe();
    if (this.Dataevents) this.Dataevents.unsubscribe();
    if (this.Uievents) this.Uievents.unsubscribe();
    this.store.alert = this.alert;
    this.store.alertMsg = this.alertMsg;
  }


  ngAfterViewInit() {}

  ngOnInit() {
    this.alert = this.store.alert;
    this.alertMsg = this.store.alertMsg;
    this.alertBtn = false;
    this.calibrationText = "";
    this.measureButton = this.lang.Language.start;

    if(this.rpi.rpiData.measure_started === 1){this.stopBtn()}
    if(this.rpi.rpiData.measure_started === 0){this.startBtn()}
    if(this.rpi.rpiData.measure_started === 0){this.stopTxt()}
    if(this.rpi.rpiData.measure_started === 1){this.startTxt()}
    if(this.rpi.rpiData.measure_started === 1){this.measureStartedYet = true;}
    if (this.store.calibStartedYet === true) { this.measureStartedYet = false; this.startTxt()}

    if(this.measureStartedYet) {
      this.warnStartTxt();
      this.stopBtn()}

    this.menu.Clear();
    this.menu.AddButton(this.b1,null);
    this.menu.AddButton(this.b2,null);
    this.menu.Highlight(0);


   this.events =  this.rpi.subjectEvents().subscribe((calib)=>{
      console.log(calib);

      if (calib === RpiEvents.button_menu)
        this.menu.NextElem();
      else
        if (calib === RpiEvents.button_ok)
          this.menu.ClickElem();

      if (calib === RpiEvents.calibration_end)
      {
        this.showMessage(this.lang.Language.alert_cal_end);
        this.store.calibStartedYet = false;
        this.stopTxt();
        this.startBtn();

      }

      if (calib === RpiEvents.calibration_started)
      {
        this.showMessage(this.lang.Language.alert_cal_started);
        this.stopBtn();
        this.startTxt();
        this.store.calibStartedYet = true;

        console.log(this.rpi.rpiData.aw_finale);

        this.calibStage++;

        if(this.calibStage === 2) {
          let val = this.rpi.rpiData.aw_finale;
          this.store.calibAw = val
        }
        if(this.calibStage > 2){
          let val = this.rpi.rpiData.aw_finale;
          this.store.calibAw2 = val
        }
      }

      if (calib === RpiEvents.calibration_error)
      {
        this.showMessage(this.lang.Language.alert_err);
        this.store.calibStartedYet = false;
        this.stopTxt();
        this.startBtn();
        this.calibStage = null;
      }

     if (calib === RpiEvents.measure_error)
     {
       this.showMessage(this.lang.Language.alert_err);
       this.store.calibStartedYet = false;
       this.stopTxt();
       this.startBtn();
       this.calibStage = null;
     }

     if (calib === RpiEvents.measure_end)
     {
       this.showMessage(this.lang.Language.alert_err);
       this.store.calibStartedYet = false;
       this.stopTxt();
       this.startBtn();



       console.log(this.rpi.rpiData.aw_finale);

       if(this.calibStage === 1) {
         let val = this.rpi.rpiData.aw_finale;
         this.store.calibAw = val
       } else {
         let val = this.rpi.rpiData.aw_finale;
         this.store.calibAw2 = val
       }

     }

      if (calib === RpiEvents.calibration_error_close_salt)
      {
        this.showMessage(this.lang.Language.alert_err_closeSalt);
        this.store.calibStartedYet = false;
        this.stopTxt();
        this.startBtn();
        this.calibStage = null;
      }

      if (calib === RpiEvents.calibration_error_salt)
      {
        this.showMessage(this.lang.Language.alert_err_salt);
        this.store.calibStartedYet = false;
        this.stopTxt();
        this.startBtn();
        this.calibStage = null;
      }

      });


   this.Uievents = this.rpi.subjectUi().subscribe((calib) => {
      console.log(calib);
       if (calib.message === RpiUiEvents.ui_salt_1 )
       {
         this.showMessage(this.lang.Language.alert_salt_1,true);
         this.store.calibStartedYet = false;
         this.stopTxt();
         this.startBtn();
       }

       if (calib.message === RpiUiEvents.ui_magnetic_down)
       {
         this.showMessage(this.lang.Language.alert_head_down,true);
         this.store.calibStartedYet = false;
         this.stopTxt();
         this.startBtn();
         this.calibStage = null;
       }

      if (calib.message === RpiUiEvents.ui_aborted_magnetic)
      {
        this.showMessage(this.lang.Language.alert_stop,true);
        this.store.calibStartedYet = false;
        this.stopTxt();
        this.startBtn();
        this.calibStage = null;

      }

      if (calib.message === RpiUiEvents.ui_salt_2)
      {
        this.showMessage(this.lang.Language.alert_salt_2,true);
        this.store.calibStartedYet = false;
        this.stopTxt();
        this.startBtn();

      }

      if (calib.message === RpiUiEvents.ui_clean_mirror)
      {
        this.showMessage(this.lang.Language.alert_err_mirror,true);
        this.store.calibStartedYet = false;
        this.stopTxt();
        this.startBtn();
        this.calibStage = null;

      }



    });

  }

  onCalibration() {
    //this.menu.setActive(false);
    this.saltReset();

    if (this.rpi.rpiData.measure_started){
    this.rpi.cmd(RpiCommands.measure,"stop").subscribe((resp)=>{
      this.showMessage(this.lang.Language.alert_stop);

      this.store.calibStartedYet = false;
      this.startBtn();
      this.stopTxt();
      this.calibStage = 0;
    });
  }
      this.rpi.cmd(RpiCommands.measure, "calib").subscribe((resp) => {




      });

    }
  public onOk(event:string) {
    this.rpi.cmd(RpiCommands.dialog,0);
    console.log("dialog button-ok");
    this.closeMessage();
  }

 public onCancel(event:string) {
    this.rpi.cmd(RpiCommands.dialog,0);
    console.log("dialog button-cancel");
    this.store.calibStartedYet = false;
   this.startBtn();
   this.closeMessage();
   this.calibStage = null;
  }

  public showMessage(text:string,dialog:boolean = false){
    console.log("showMessage:[%s] dialog:[%s]",text,dialog);
    if (dialog)
    this.menu.setActive(false);

    this.alertSalt = dialog;
    this.alert = true;
    this.alertMsg= text;


    //deactive
   /* if (!dialog) {
      setTimeout(() => {
        if (this.alertBtn === false)
          this.alert = false;
      }, 3500);
    } */
  }

  public closeMessage(){
    this.menu.setActive(true);
    this.menu.Highlight(0);
    this.alert = false;
    this.alertMsg = "";
    this.alertBtn = false;
  }

  public stopBtn() {
    this.measureButton = this.lang.Language.stop
  }

  public startBtn() {
    this.measureButton = this.lang.Language.start
  }

  public warnStartTxt() {
    this.calibrationText = this.lang.Language.calib_txt_warn;
  }

  public startTxt() {
    this.calibrationText =this.lang.Language.calib_txt_on;
  }

  public stopTxt(){
    this.calibrationText =this.lang.Language.calib_txt_off;
  }

  public onSalt(ev) {

    if(this.calibStage === 1) {
      this.store.calibAw = 0;
      this.store.calibAw2 = 0;
      this.store.sampleSaltValue = ev.val;
    } else {
      this.store.sampleSaltValue2 = ev.val;
    }

    this.rpi.cmd(RpiCommands.dialog,ev.index);
    console.log("Salt "+ev+"selected");
    this.closeMessage();
  }

  saltReset() {
    this.store.calibAw = 0;
    this.store.calibAw2 = 0;
    this.store.sampleSaltValue = 0;
    this.store.sampleSaltValue2 = 0;
  }

  }


