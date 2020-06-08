import {
  Component, OnInit, ViewChild, ElementRef, OnDestroy, AfterViewInit, DoCheck, ViewChildren,
  QueryList, ContentChildren
} from '@angular/core';
import {RpiCommands, RpiConfig, RpiEvents, RpiService, RpiUiEvt} from "../rpi.service";
import * as Chart from 'chart.js';
import {Menu} from "../menu";
import {Subscription} from "rxjs/Subscription";
import {LenguageService} from "../lenguage.service";
import {StorageService} from "../storage.service";
import {current} from "codelyzer/util/syntaxKind";
import {repeat} from "rxjs/operator/repeat";
import {sample} from "rxjs/operator/sample";
import {element} from "protractor";
import {environment} from "../../environments/environment";


@Component({
  selector: 'app-mesurement-menu',
  templateUrl: './mesurement-menu.component.html',
  styleUrls: ['./mesurement-menu.component.css']
})
export class MesurementMenuComponent implements OnInit,OnDestroy,AfterViewInit,DoCheck {


  constructor(public rpi:RpiService,public lang:LenguageService,public store:StorageService) { }
  public menu:Menu = new Menu();

  @ViewChild('b1') b1:ElementRef;
  @ViewChild('b2') b2:ElementRef;
  @ViewChild('b3') b3:ElementRef;

  @ViewChildren ('sample') s1:QueryList<ElementRef>;


   graphData= [];
   alertMsg:string = this.store.alertMsg;
   alert:boolean = this.store.alert;
   measureButton = this.lang.Language.start;
   chart:any;
   culo:string;
   btn_start = "start";
   startedMeasure:number;
   Samples:Array<any> = [];
   sampleOk:boolean = false;
   sampleNoOK:boolean = false;
   XiOk:boolean = false;
   whiteOk:boolean = false;
   blueOK: boolean = false;
   doubleClick = false;
   noBlink1;
   noBlink2;
   noBlink3;
   dataUI = [];
   currentEvent:any;
   sampleBtns:Array<any> = [];

   comandAW = 'aw';





  private events:Subscription = null;
  private Dataevents:Subscription = null;
  private Uievents:Subscription = null;

   ngOnDestroy(){
     if (this.events) this.events.unsubscribe();
     if (this.Dataevents) this.Dataevents.unsubscribe();
     if (this.Uievents) this.Uievents.unsubscribe();
   }

   ngAfterViewInit(){
     this.initChart("myChart");
     this.Dataevents = this.rpi.subjectDataEvent().subscribe(()=>{
       this.chart.data.datasets[0].data = this.rpi.surface_data.dataList;
       this.chart.update(0);
     });
     if (this.rpi.rpiData.measure_started === 0){this.measureButton=this.lang.Language.start}
     if (this.rpi.rpiData.measure_started === 1){this.measureButton=this.lang.Language.stop}

    //this.sampleBtns[0] = this.s1.toArray()[0];

 /*    console.log(
 /*    console.log(
       this.s1.map((item)=> item)
   );
     console.log(this.s1);*/

     this.s1.changes.subscribe(()=>{
          console.log("aw button created");

          this.s1.forEach((element:ElementRef)=>{
            this.menu.AddButton(element,null);
          });
      });

     this.menu.Clear();
     this.menu.AddButton(this.b1,null);
     this.menu.AddButton(this.b2,null);
     this.menu.AddButton(this.b3,null);

     this.menu.Highlight(0);

console.log(this.menu)
   }

   ngDoCheck() {}


  ngOnInit() {

    this.rpi.readConfig(RpiConfig.UI_STORAGE).subscribe((value)=>{
      this.dataUI = JSON.parse(value);
      console.log(this.dataUI);
      this.store.xiLimit = this.dataUI[4].value;
      console.log( this.store.xiLimit)
    });

     if(this.rpi.rpiData.measure_started === 1) {
       this.blink(this.currentEvent)
     }


     this.Samples.push(
       {
         name:"",
         aw:"",
         comparation:"",
         classStyle: "colored-1",
         tabIndex: 4,

       },
       {
         name:"",
         aw:"",
         comparation:"",
         classStyle: "colored-2",
         tabIndex: 4,
         href: "s2",
       },
       {
         name:"",
         aw:"",
         comparation:"",
         classStyle: "colored-3",
         tabIndex: 4,
         href: "s3",
       },
       {
         name:"",
         aw:"",
         comparation:"",
         classStyle: "colored-4",
         tabIndex: 4,
         href: "s4",
       },
       );

    this.events = this.rpi.subjectEvents().subscribe((data)=>{

      this.currentEvent = data;
      this.blink(this.currentEvent);


      if (data === RpiEvents.button_menu){
        this.menu.NextElem();
      }else
      if (data === RpiEvents.button_ok){
        this.menu.ClickElem();
      }



      if (data === RpiEvents.aw){
        this.measureButton = this.lang.Language.stop;
        this.showMessage(this.lang.Language.alert_start);

      }

      if (data === RpiEvents.measure_aborted || data === RpiEvents.measure_end || data === RpiEvents.measure_error){
        this.measureButton = this.lang.Language.start;



        if (data === RpiEvents.measure_aborted)
          this.showMessage(this.lang.Language.alert_stop);



        if (data === RpiEvents.measure_end) {
          this.showMessage(this.lang.Language.alert_measure_end);
          this.blueOK = true;
          this.AwComparation();
        }

        if (data === RpiEvents.measure_end && this.rpi.rpiData.fan > 0) {
          this.showMessage(this.lang.Language.alert_measure_end);
          this.blueOK = true;
          this.AwComparation();
        }



        if (data === RpiEvents.measure_error)
          this.showMessage(this.lang.Language.alert_measure_error);

      }

    });

   this.events = this.rpi.readConfig(RpiConfig.UI_STORAGE).subscribe((val)=>{
     let json = JSON.parse(val);

     this.Samples[0] = Object.assign(this.Samples[0],json[0]);
     this.Samples[1] = Object.assign(this.Samples[1],json[1]);
     this.Samples[2] = Object.assign(this.Samples[2],json[2]);
     this.Samples[3] = Object.assign(this.Samples[3],json[3]);

     //console.log(...this.Samples); //Object.assign([],this.Samples);

      if (  this.Samples[0].name === "" &&
     this.Samples[1].name === "" &&
     this.Samples[2].name === "" &&
     this.Samples[3].name === "") {
        this.store.sampleAw = "--";
      this.store.sampleName = "--";
      this.store.sampleToggle = 0;}

    });

    this.Uievents = this.rpi.subjectUi().subscribe((data)=>{
      console.log(data);
    });
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
            },
            {
              label: this.lang.Language.graph_title_obj,
              fill: false,
              lineTension: 0.2, //0.1
              backgroundColor: "rgba(75,192,192,0.4)",
              borderColor: "#c04b4b",
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
              data: this.rpi.object_temp_data.dataList  //this.rpi.surface_data.dataList,
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

  StartMeasure(){
    if(!this.doubleClick) {
      setTimeout( ()=> {
        this.StartAw()
      },1000);
      this.doubleClick = false;
    }
  }

  StartKinetic() {
    this.doubleClick = true;
    this.comandAW = 'kinetics';
    this.StartAw()
  }

   StartAw(){
    this.AwReset();


    if (this.rpi.rpiData.measure_started){
      this.rpi.cmd(RpiCommands.measure,"stop").subscribe((resp)=>{
        if (resp.response == null){
          this.sampleOk = false;
          this.sampleNoOK = false;
          console.log("measure stopping");
          this.AwReset();


        }
      });
    }
    else


     /* if (!environment.production){
      this.comandAW = RpiCommands.testaw
      }*/

     this.rpi.cmd(RpiCommands.measure,this.comandAW).subscribe((resp)=>{



      if(this.rpi.currentAw)
        if (resp.response == null){
          console.log("measure starting");
        } else
        if (resp.response === "magnetic" ) {
          this.showMessage(this.lang.Language.alert_head_down);
        }

    });

     this.comandAW = 'aw';

  }

  showMessage(str:string){
    this.store.alertMsg = str;
    this.store.alert = true;
    this.alertMsg = this.store.alertMsg;
    this.alert = true;
    this.alertMsg = str
  }

  sampleSelect(aw,name,comp,i){

    if(this.store.sampleToggle === i ) {
      this.store.sampleName = "----";
      this.store.sampleAw = "Aw";
      if (this.store.sampleAw ="Aw"){
        this.store.sampleToggle = -1;
      }

    } else { this.store.sampleAw = aw;
      this.store.sampleName = name;
      this.store.sampleComp = comp;
      this.store.sampleToggle = i;
      }


  }

  blink(data){
    let xiLimit;
      xiLimit = this.store.xiLimit;

    if(data !== 'aw' || this.rpi.rpiData.measure_started === 0){
      this.XiOk = false;
      this.whiteOk = false;
    }
    if (data === 'aw' || this.rpi.rpiData.measure_started === 1){
      if(this.rpi.rpiData.xi > xiLimit || this.rpi.rpiData.xi < -xiLimit && this.rpi.rpiData.measure_started === 1) {
        this.XiOk = false;
        this.whiteOk = false;
        this.noBlink3 = setTimeout(()=> {this.blink(data)},2000);
      }
      if(this.rpi.rpiData.xi <= xiLimit && this.rpi.rpiData.xi >= -xiLimit ){
          this.XiOk = true;
          this.whiteOk = false;
         this.noBlink2 =  setTimeout(()=>this.white(),1500);
         this.noBlink1 = setTimeout(()=> {this.blink(this.currentEvent)},2000);


      }
    }
     }

   AwComparation(){
     if (this.store.sampleComp === ">") {
       if (this.rpi.currentAw > +this.store.sampleAw) {
         this.sampleOk = true;
         this.sampleNoOK = false;
         this.blueOK = false;
       }
       if (this.rpi.currentAw < +this.store.sampleAw) {
         this.sampleOk = false;
         this.sampleNoOK = true;
         this.blueOK = false;
       }
     }

     if (this.store.sampleComp === "<") {
       if (this.rpi.currentAw < +this.store.sampleAw) {
         this.sampleOk = true;
         this.sampleNoOK = false;
         this.blueOK = false;
       }
       if (this.rpi.currentAw > +this.store.sampleAw) {
         this.sampleOk = false;
         this.sampleNoOK = true;
         this.blueOK = false;
       }
     }
   }

   AwReset(){
     this.sampleNoOK = false;
     this.sampleOk = false;
     this.whiteOk = false;
     this.XiOk = false;
     this.blueOK = false;
   }

  white() {
    this.whiteOk = true
  }
}





