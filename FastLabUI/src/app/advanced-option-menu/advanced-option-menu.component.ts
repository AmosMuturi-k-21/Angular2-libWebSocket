import {Component, ElementRef, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {Rpi_COMPARATION_MODE_str, RpiConfig, RpiEvents, RpiService} from "../rpi.service";
import {LenguageService} from "../lenguage.service";
import {Menu} from "../menu";
import {Observable} from "rxjs/Observable";
import {Subscription} from "rxjs/Subscription";
import {Subject} from "rxjs/Subject";

@Component({
  selector: 'app-advanced-option-menu',
  templateUrl: './advanced-option-menu.component.html',
  styleUrls: ['./advanced-option-menu.component.css']
})
export class AdvancedOptionMenuComponent implements OnInit, OnDestroy {

  buttons:Array<any> = [];
  unsub:Subscription = null;

  value:number = 0;
  input$ = new Subject<number>();


  constructor(public rpi:RpiService, public lang:LenguageService) { }
  public menu:Menu = new Menu();

  public MenuType = MenuType;

  @ViewChild('b1') b1:ElementRef;
  @ViewChild('temperature') temperature:ElementRef;
  @ViewChild('name1') name1:ElementRef;
  @ViewChild('name2') name2:ElementRef;
  @ViewChild('name3') name3:ElementRef;
  @ViewChild('name4') name4:ElementRef;
  @ViewChild('select1') select1:ElementRef;
  @ViewChild('select2') select2:ElementRef;
  @ViewChild('select3') select3:ElementRef;
  @ViewChild('select4') select4:ElementRef;
  @ViewChild('aw1') aw1:ElementRef;
  @ViewChild('aw2') aw2:ElementRef;
  @ViewChild('aw3') aw3:ElementRef;
  @ViewChild('aw4') aw4:ElementRef;


  public Rpi_COMPARATION_MODE_list;

  ngOnInit() {

    this.menu.Clear();
    this.menu.AddButton(this.temperature,null);

    this.menu.AddButton(this.b1,null);
    this.menu.AddButton(this.name1,null);
    this.menu.AddButton(this.select1,null);
    this.menu.AddButton(this.aw1,null);

    this.menu.AddButton(this.name2,null);
    this.menu.AddButton(this.select2,null);
    this.menu.AddButton(this.aw2,null);

    this.menu.AddButton(this.name3,null);
    this.menu.AddButton(this.select3,null);
    this.menu.AddButton(this.aw3,null);

    this.menu.AddButton(this.name4,null);
    this.menu.AddButton(this.select4,null);
    this.menu.AddButton(this.aw4,null);
    this.menu.Highlight(0);

    this.rpi.subjectEvents().subscribe((evt)=>{

      if (evt === RpiEvents.button_menu){
        this.menu.NextElem();
      }else
      if (evt === RpiEvents.button_ok){
        this.menu.ClickElem();
      }

    });

    this.Rpi_COMPARATION_MODE_list = Rpi_COMPARATION_MODE_str;

    this.buttons.push(
      {
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
        value: 50,
      }
      );

   /* let Sample_1={
      name:'ciao',
      aw:'',
      btn: 1,
    };

    let Sample_2={
      name:'ciao',
      aw:'',
      btn: 2,
    };

    let Sample_3={
      name:'ciao',
      aw:'',
      btn: 3,
    };

    let Sample_4={
      name:'ciao',
      aw:'',
      btn: 4,
    }; */



  this.unsub =  this.rpi.readConfig(RpiConfig.UI_STORAGE).subscribe((value)=>{
      console.log(value);
      console.log(JSON.parse(value));

      if(JSON.parse(value) === undefined || JSON.parse(value) === null) {
        this.rpi.writeConfig(RpiConfig.UI_STORAGE,JSON.stringify(this.buttons)).subscribe(()=>{
          console.log("config writed!");
        });
      } else {
        this.buttons = JSON.parse(value);
      }

    });

    this.rpi.readConfig(RpiConfig.SAMPLE_TEMP).subscribe((config)=>{
      this.value = config;
    });

    this.input$
      .auditTime(1000)
      .subscribe(x => {
        this.value=x;
        this.rpi.writeConfig(RpiConfig.SAMPLE_TEMP,this.value).subscribe(()=>{
        });
      });

  }

  onClickSlider(evt:any){
    if (evt.isTrusted) return; //mouse

    this.value+=1;
    if (this.value > 30)
      this.value = 10;

    this.rpi.writeConfig(RpiConfig.SAMPLE_TEMP,this.value).subscribe(()=>{
    });

  }

  onClickSelect(ele:HTMLSelectElement,type:MenuType,evt:any){

    if (!evt.isTrusted) { //mouse event
      const size = ele.length;
      ele.selectedIndex++;
      if (ele.selectedIndex >= size || ele.selectedIndex === -1)
        ele.selectedIndex = 0;
    } else {

     /* switch(type){
        case MenuType.comparation_mode_1:

               let mode = false;
               mode = !mode;
             if(mode){
               this.buttons[0].comparation =">"
             }
            if(!mode){
              this.buttons[0].comparation ="<"
            }

          break;}
      switch(type){
        case MenuType.comparation_mode_2:
          let mode = false;
          mode = !mode;
          if(mode){
            this.buttons[1].comparation =">"
          }
          if(!mode){
            this.buttons[1].comparation ="<"
          }


          break;}
      switch(type){
        case MenuType.comparation_mode_3:
          let mode = false;
          mode = !mode;
          if(mode){
            this.buttons[2].comparation =">"
          }
          if(!mode){
            this.buttons[2].comparation ="<"
          }

          break;}
      switch(type){
        case MenuType.comparation_mode_4:
          let mode = false;
          mode = !mode;
          if(mode){
            this.buttons[3].comparation =">"
          }
          if(!mode){
            this.buttons[3].comparation ="<"
          }

          break;} */
    }


  }

  ngOnDestroy() {
    this.rpi.writeConfig(RpiConfig.UI_STORAGE,JSON.stringify(this.buttons)).subscribe(()=>{
        console.log("config writed!");
      },
      (err)=>console.log(err));

    this.unsub.unsubscribe()
  }

}

export enum MenuType{
  language,
  comparation_mode_1,
  comparation_mode_2,
  comparation_mode_3,
  comparation_mode_4,
}



