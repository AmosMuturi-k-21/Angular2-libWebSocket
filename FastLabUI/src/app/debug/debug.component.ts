import {Component, ElementRef, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {RpiCommands, RpiConfig, RpiEvents, RpiService} from "../rpi.service";
import {LenguageService} from "../lenguage.service";
import {Menu} from "../menu";
import {Subscription} from "rxjs/Subscription";
import * as Chart from 'chart.js';
import {environment} from "../../environments/environment.prod";
import {FormBuilder, FormGroup} from "@angular/forms";
import {FastLabCalibration} from "../calibration/calibration";
import {StorageService} from "../storage.service";


@Component({
  selector: 'app-debug',
  templateUrl: './debug.component.html',
  styleUrls: ['./debug.component.css']
})
export class DebugComponent implements OnInit,OnDestroy {

  public menu:Menu = new Menu();
  @ViewChild('b1') b1:ElementRef;
  @ViewChild('b2') b2:ElementRef;
  @ViewChild('b3') b3:ElementRef;
  @ViewChild('b4') b4:ElementRef;
  @ViewChild('b5') b5:ElementRef;
  @ViewChild('b6') b6:ElementRef;
  @ViewChild('b7') b7:ElementRef;
  @ViewChild('b8') b8:ElementRef;
  //@ViewChild('slopeinfrared') slopeinfrared:ElementRef;
  @ViewChild('bslopemirror') bslopemirror:ElementRef;
  @ViewChild('lcdid') lcdid:ElementRef;

  @ViewChild('deltaaw') deltaaw:ElementRef;
  @ViewChild('slopeaw') slopeaw:ElementRef;

  @ViewChild('sampletemp') sampletemp:ElementRef;

  calibration:FormGroup;
  awCalib:FastLabCalibration;

  btn_start = "start";
  measureButton = this.lang.Language.start;
  comandAW = "aw";
  unsub:Subscription;

  private event:Subscription;
  private evData:Subscription;

  public values:{key:string,value:any}[] = [];
  public ValuesTop:{key:string,value:any}[] = [];
  public ValueBottom:{key:string,value:any}[] = [];

  dataUI = [];

  public tvalue = 0;
  public mvalue = 0;
  public ivalue = 0;
  public lvalue = 0;
  public bvalue = 0;
  public msvalue = 0;
  public stvalue = 0;
  public lcdidvalue = '';
  public xivalue = 50;
  public awdelta = 0;
  public awslope = 0;
  public sampletempvalue = 0;
  public InfraredSlope = 0;

  public tmiravgsample = 0;
  public tmiravgfreq = 0;
  public threflexion = 0;

  public autoshiftval = true;

  chart:any;

  public ValueMinTemp = -300;
  public ValueMaxTemp = 300.4;

  public ValueMinSlope = -30;
  public ValueMaxSlope = 30.4;


  constructor(public rpi:RpiService,public lang:LenguageService, private formBuilder:FormBuilder,private storage:StorageService) { }

  ngOnDestroy(){
    if (this.event) this.event.unsubscribe();
    if (this.evData) this.evData.unsubscribe();
    this.unsub.unsubscribe();
    this.dataUI.splice(4,this.dataUI.length);
    this.dataUI.push({
      val: this.storage.xiLimit
    });
    this.rpi.writeConfig(RpiConfig.UI_STORAGE,JSON.stringify(this.dataUI)).subscribe(()=>{
      },
      (err)=>console.log(err));
    console.log(this.dataUI)
  }

  ngOnInit() {

    this.dataUI.push(  {
        name:'',
        aw: '',
        comparation:'',
      },

      {
        name:'',
        aw: '',
        comparation:'',
      },
      {
        name:'',
        aw:'',
        comparation:'',
      },
      {
        name:'',
        aw:'',
        comparation:'',
      },
      {
        val: 50
      },);

    this.calibration = this.formBuilder.group({
        tsurf_1:[],
        tsurf_2:[],
        aw_mes_1:[],
        aw_mes_2:[],
        aw_rech_1:[],
        aw_rech_2:[],
        slope:[1],
        delta:[0],

        /*tsurf_1:[20.8],
        tsurf_2:[21.3],
        aw_mes_1:[0.690],
        aw_mes_2:[0.995],
        aw_rech_1:[ 0.753],
        aw_rech_2:[ 0.753],
        slope:[1],
        delta:[0],*/
    }
    );

  this.unsub = this.rpi.subjectEvents().subscribe((data)=>{

      if (data === RpiEvents.measure_aborted || data === RpiEvents.measure_end || data === RpiEvents.measure_error){
        this.measureButton = this.lang.Language.start;}

      });



    if(this.rpi.rpiData.measure_started === 0){
      this.measureButton = this.lang.Language.start;
      this.btn_start = "start"
    }

    if(this.rpi.rpiData.measure_started === 1){
      this.measureButton = this.lang.Language.stop;
      this.btn_start = "stop"
    }



    this.menu.Clear();
    /*menu disabled*/
    /*this.menu.AddButton(this.b1,null);
    this.menu.AddButton(this.bslopemirror,null);
    this.menu.AddButton(this.b2,null);
    this.menu.AddButton(this.b3,null);
    this.menu.AddButton(this.b4,null);
    this.menu.AddButton(this.b5,null);
    this.menu.AddButton(this.b6,null);
    this.menu.AddButton(this.b7,null);
    this.menu.Highlight(0);*/


    this.initChart("myChart");

    this.event=this.rpi.subjectEvents().subscribe((evt)=>{

      if (evt === RpiEvents.button_menu){
        this.menu.NextElem();
      }else
      if (evt === RpiEvents.button_ok){
        this.menu.ClickElem();
      }

    });

    this.evData = this.rpi.subjectDataEvent().subscribe(()=>{

      this.chart.data.datasets[0].data = this.rpi.surface_data.dataList;
      this.chart.update(0);

        this.ValuesTop = [];
        this.ValueBottom = [];
        /*for (let key of Object.keys(this.rpi.rpiData).sort()){
            this.values.push({key:key,value:this.rpi.rpiData[key]});
        }*/
      this.ValuesTop.push({key:'Time',value:this.rpi.rpiData.time});
      this.ValuesTop.push({key:'Time measure',value:this.rpi.rpiData.time_measure});
      this.ValuesTop.push({key:'Reflexion mV',value:this.rpi.rpiData.reflexion_mV});
      this.ValuesTop.push({key:'Reflexion',value:this.rpi.rpiData.reflexion});
      this.ValuesTop.push({key:'Detection',value:this.rpi.rpiData.detection});
      this.ValuesTop.push({key:'Surface temp',value:this.rpi.rpiData.t_surface});
      this.ValuesTop.push({key:'Mirror temp',value:this.rpi.rpiData.t_mirror});
      this.ValuesTop.push({key:'Peltier',value:this.rpi.rpiData.peltier});
      this.ValuesTop.push({key:'Aw inst',value:(this.rpi.rpiData.aw_inst  /1000)});
      this.ValuesTop.push({key:'Aw medium',value:(this.rpi.rpiData.aw_medium  /1000)});
      this.ValuesTop.push({key:'Aw final',value:(this.rpi.rpiData.aw_finale /1000)});
      this.ValuesTop.push({key:'Aw int',value:(this.rpi.rpiData.aw_int  /1000)});
      this.ValuesTop.push({key:'Xi',value:this.rpi.rpiData.xi});
      this.ValuesTop.push({key:'Step',value:this.rpi.rpiData.current_task});
      
      this.ValueBottom.push({key:'Aw flash',value:this.rpi.rpiData.aw_flash});
      this.ValueBottom.push({key:'Fan',value:this.rpi.rpiData.fan});
      this.ValueBottom.push({key:'Xi',value:this.rpi.rpiData.clean_mirror});
      this.ValueBottom.push({key:'Xi',value:this.rpi.rpiData.measure_started});
      this.ValueBottom.push({key:'Xi',value:this.rpi.rpiData.current_task});
      this.ValueBottom.push({key:'Xi',value:this.rpi.rpiData.button_magnetic});
      this.ValueBottom.push({key:'Xi',value:this.rpi.rpiData.button_ok});
      this.ValueBottom.push({key:'Xi',value:this.rpi.rpiData.button_menu});
      this.ValueBottom.push({key:'adc0',value:this.rpi.rpiData.adc0});
      this.ValueBottom.push({key:'adc1',value:this.rpi.rpiData.adc1});
      this.ValueBottom.push({key:'adc2',value:this.rpi.rpiData.adc2});
      this.ValueBottom.push({key:'adc3',value:this.rpi.rpiData.adc3});
      this.ValueBottom.push({key:'adc4',value:this.rpi.rpiData.adc4});
      this.ValueBottom.push({key:'adc5',value:this.rpi.rpiData.adc5});
      this.ValueBottom.push({key:'adc6',value:this.rpi.rpiData.adc6});
      this.ValueBottom.push({key:'adc7',value:this.rpi.rpiData.adc7});

      //console.log("data push!");

    });


    this.rpi.readConfig(RpiConfig.DELTA_TEMP_SURF).subscribe((config)=>{
      this.tvalue = config;

      //update delta on the form
     /* this.calibration = this.formBuilder.group({
          delta:[this.tvalue],
        }
      );*/

    });
    this.rpi.readConfig(RpiConfig.DELTA_TEMP_MIRROR).subscribe((config)=>{
      this.mvalue = config;
    });
    this.rpi.readConfig(RpiConfig.DELTA_VALUE_IR).subscribe((config)=>{
      this.ivalue = config;
    });
    this.rpi.readConfig(RpiConfig.BUZZ_FREQ).subscribe((config)=>{
      this.bvalue = config;
    });
    this.rpi.readConfig(RpiConfig.LCD_REFRESH).subscribe((config)=>{
      this.lvalue = config;
    });
    this.rpi.readConfig(RpiConfig.LCD_ID).subscribe((config)=>{
      this.lcdidvalue = this.NumToStrHex(config);
    });
    this.rpi.readConfig(RpiConfig.SAMPLE_TEMP).subscribe((config)=>{
      this.sampletempvalue = config;
    });
    this.rpi.readConfig(RpiConfig.SLOPE_TEMP_SURF).subscribe((config)=>{
      this.stvalue = config;

      //update slope on the form
      /*this.calibration = this.formBuilder.group({
          slope:[this.stvalue],
        }
      );*/

    });
    this.rpi.readConfig(RpiConfig.SLOPE_TEMP_MIRROR).subscribe((config)=>{
      this.msvalue = config;
    });
    this.rpi.readConfig(RpiConfig.SLOPE_VALUE_IR).subscribe((config)=>{
      this.InfraredSlope = config;
    });

    //aw
    this.rpi.readConfig(RpiConfig.SLOPE_AW).subscribe((config)=>{
      this.awslope = config;
    });
    this.rpi.readConfig(RpiConfig.DELTA_AW).subscribe((config)=>{
      this.awdelta = config;
    });

    this.rpi.readConfig(RpiConfig.FREQUENCY_TMIR).subscribe((config)=>{
      this.tmiravgfreq = config;
    });
    this.rpi.readConfig(RpiConfig.AVERAGE_TMIR).subscribe((config)=>{
      this.tmiravgsample = config;
    });
    this.rpi.readConfig(RpiConfig.SEUIL).subscribe((config)=>{
      this.threflexion = config;
    });
    this.rpi.readConfig(RpiConfig.MIRROR_AUTO_SHIFT).subscribe((config)=>{
      console.log(config);
      this.autoshiftval = config===0?false:true;
    });

    this.rpi.readConfig(RpiConfig.UI_STORAGE).subscribe((value)=>{

      if(JSON.parse(value).length < 5) {
        this.xivalue = this.dataUI[4].val
      } else {
        this.dataUI = JSON.parse(value);
        this.xivalue = this.dataUI[4].val
      }
    });



  }

  updateLcd(str:string){
    const val = this.StrHexToNum(str);
    if (val == null) return;
    this.rpi.writeConfig(RpiConfig.LCD_ID,val).subscribe(()=>{
      console.log("update LCDID to %s",val);
    });
  }

  sampleTemperature(str:string){
    const val = this.StrHexToNum(str);
    if (val == null) return;
    this.rpi.writeConfig(RpiConfig.SAMPLE_TEMP,val).subscribe(()=>{
      console.log("update sample temperature to %s",val);
    });
  }

  setTmirAvgSample(str:string){
    const val = this.StrHexToNum(str);
    if (val == null) return;
    this.rpi.writeConfig(RpiConfig.AVERAGE_TMIR,val).subscribe(()=>{
    });
  }

  setTmirAvgFreq(str:string){
    const val = this.StrHexToNum(str);
    if (val == null) return;
    this.rpi.writeConfig(RpiConfig.FREQUENCY_TMIR,val).subscribe(()=>{
    });
  }

  setThReflexion(str:string){
    str = str.toLowerCase().trim();
    if (str == null || str === '') return;
    const val = Number.parseFloat(str);
    if (val == null) return;
    this.rpi.writeConfig(RpiConfig.SEUIL,val).subscribe(()=>{
    });
  }

  setAutoshiftmirror(checked:boolean){
    if (checked == null) return;
    const val = checked?1:0;
    this.rpi.writeConfig(RpiConfig.MIRROR_AUTO_SHIFT,val).subscribe(()=>{
    });
  }

  StrHexToNum(str:string):number | null{
    str = str.toLowerCase().trim();
    if (str.length === 0) return null;
    //if (str.charAt(0) !== '0' && str.charAt(0) !== 'o') return null;
    if (str.indexOf('0x') === 0 || str.indexOf('ox') === 0){
       if (str.length === 2) return null;
       return Number.parseInt(str.slice(2),16);
    }
    return Number.parseInt(str,10);
  }

  NumToStrHex(value:number):string{
    return '0x'+value.toString(16);
  }

  onClickSlider(slider:number,evt:any,value:number = null){
    switch (slider){
      case 0: //DELTA_TEMP_SURF
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.tvalue + 0.5;
        }else
        if (value == null) value = this.tvalue;

        if (value > this.ValueMaxTemp) value = this.ValueMinTemp;
        this.tvalue = value;
        this.rpi.writeConfig(RpiConfig.DELTA_TEMP_SURF,value).subscribe(()=>{
        });
      break;
      case 1: //DELTA_TEMP_MIRROR
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.mvalue + 0.5;
        }else
        if (value == null) value = this.mvalue;

        if (value > this.ValueMaxTemp) value = this.ValueMinTemp;
        this.mvalue = value;
        this.rpi.writeConfig(RpiConfig.DELTA_TEMP_MIRROR,value).subscribe(()=>{
        });
        break;
      case 2:
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.ivalue + 0.5;
        }else
        if (value == null) value = this.ivalue;

        if (value > 100.9) value = -100;
        this.ivalue = value;
        this.rpi.writeConfig(RpiConfig.DELTA_VALUE_IR,value).subscribe(()=>{
        });
        break;
      case 3:
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.bvalue + 100;
        }else
        if (value == null) value = this.bvalue;

        if (value > 5099) value = 100;
        this.bvalue = value;
        this.rpi.writeConfig(RpiConfig.BUZZ_FREQ,value).subscribe(()=>{
        });
        break;
      case 4:
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.lvalue + 50;
        }else
        if (value == null) value = this.lvalue;

        if (value > 1049) value = 50;
        this.lvalue = value;
        this.rpi.writeConfig(RpiConfig.LCD_REFRESH,value).subscribe(()=>{
        });
        break;
      case 5: //SLOPE_TEMP_MIRROR
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.msvalue + 0.001;
        }else
        if (value == null) value = this.msvalue;

        if (value > this.ValueMaxSlope) value = this.ValueMinSlope;
        this.msvalue = value;
        this.rpi.writeConfig(RpiConfig.SLOPE_TEMP_MIRROR,value).subscribe(()=>{
        });
        break;
      case 6: //SLOPE_TEMP_SURF
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.stvalue + 0.001;
        }else
        if (value == null) value = this.stvalue;

        if (value > this.ValueMaxSlope) value = this.ValueMinSlope;
        this.stvalue = value;
        this.rpi.writeConfig(RpiConfig.SLOPE_TEMP_SURF,value).subscribe(()=>{
        });
        break;
      case 7: //xi limit
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.xivalue + 5;
        }else
        if (value == null) value = this.xivalue;

        if (value > 100) value = 100;
        this.xivalue = value;
        this.storage.xiLimit = value;
        break;
      case 8: //DELTA AW
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.awdelta + 0.5;
        }else
        if (value == null) value = this.awdelta;

        if (value > this.ValueMaxTemp) value = this.ValueMinTemp;
        this.awdelta = value;
        this.rpi.writeConfig(RpiConfig.DELTA_AW,value).subscribe(()=>{
        });
        break;
      case 9:
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.awslope + 0.001;
        }else
        if (value == null) value = this.awslope;

        if (value > this.ValueMaxSlope) value = this.ValueMinSlope;
        this.awslope = value;
        this.rpi.writeConfig(RpiConfig.SLOPE_AW,value).subscribe(()=>{
        });
        break;
      case 10: //SLOPE_VALUE_IR
        if (evt != null && !evt.isTrusted) {
          if (value == null) value = this.InfraredSlope + 0.001;
        }else
        if (value == null) value = this.InfraredSlope;

        if (value > this.ValueMaxSlope) value = this.ValueMinSlope;
        this.InfraredSlope = value;
        this.rpi.writeConfig(RpiConfig.SLOPE_VALUE_IR,value).subscribe(()=>{
        });
        break;
    }
  }

  public initChart(id:string){
    this.chart = new Chart(id, {
        type: 'line',
        data:{
          labels: Array.from(new Array(100), (x,i) => ""),
          datasets: [
            {
              label: this.lang.Language.graph_title,
              fill: false,
              lineTension: 0.2, //0.1
              backgroundColor: "rgba(75,192,192,0.4)",
              borderColor: "rgba(75,192,192,1)",
              borderCapStyle: 'butt',
              borderDash: [],
              borderDashOffset: 0.0,
              borderJoinStyle: 'miter',
              pointBorderColor: "rgba(75,192,192,1)",
              pointBackgroundColor: "#fff",
              pointBorderWidth: 1,
              pointHoverRadius: 5,
              pointHoverBackgroundColor: "rgba(75,192,192,1)",
              pointHoverBorderColor: "rgba(220,220,220,1)",
              pointHoverBorderWidth: 2,
              pointRadius: 0, //5
              pointHitRadius: 10,
              data: this.rpi.surface_data.dataList  //this.rpi.surface_data.dataList,
            }
          ]
        },
        options: {
          scales: {
            yAxes : [{
              gridLines: {
                display:false
              },
              ticks : {
                max : 45,
                min : -5
              }
            }],
            xAxes : [{
              gridLines: {
                display:false
              },
            }]
          }
        }
      }
    );
  }

  onCalib() {
    const awCal = new FastLabCalibration();
    console.log(this.calibration.value);

    const result = awCal.calibration2point(
      this.calibration.value.aw_mes_1,this.calibration.value.aw_mes_2,
      this.calibration.value.aw_rech_1,this.calibration.value.aw_rech_2
    );

    /*const result = awCal.calibrate_tsurf_aw(
      this.calibration.value.aw_mes_1,this.calibration.value.aw_mes_2,
      this.calibration.value.aw_rech_1,this.calibration.value.aw_rech_2,
      this.calibration.value.tsurf_1,this.calibration.value.tsurf_2,
      this.calibration.value.delta,this.calibration.value.slope);*/

    console.log(result);

    this.onClickSlider(SLIDER.DELTA_TEMP_SURF,null,result.delta);
    this.onClickSlider(SLIDER.SLOPE_TEMP_SURF,null,result.slope);
  }

  onCalibrationInfrared(){

  }


  StartMeasure(){

    if (this.rpi.rpiData.measure_started){

      this.rpi.cmd(RpiCommands.measure,"stop").subscribe((resp)=>{
        this.measureButton = this.lang.Language.start;
        if (resp.response == null){
          console.log("measure stopping");
          this.btn_start = 'start';
          this.measureButton = this.lang.Language.start;

        }
      });
    }
    else

    if (!environment.production){
      this.comandAW = RpiCommands.testaw
    }

    this.rpi.cmd(RpiCommands.measure,this.comandAW).subscribe((resp)=>{
      this.measureButton = this.lang.Language.stop;
      if(this.rpi.currentAw)
        if (resp.response == null){
          this.measureButton = this.lang.Language.stop;
          console.log("measure starting");
          this.btn_start = 'stop';

        }
    });
  }


}

enum SLIDER{
  DELTA_TEMP_SURF = 0,
  SLOPE_TEMP_SURF = 6,
  //@TODO fix this
  DELTA_AW = 8,
  SLOPE_AW = 9,
}
