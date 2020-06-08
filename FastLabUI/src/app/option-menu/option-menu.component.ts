import { Component, OnInit,  ViewChild, ElementRef } from '@angular/core';
import {Subject} from "rxjs/Subject";
import 'rxjs/add/operator/auditTime';
import {Menu} from "../menu";
import {
  Rpi_AUTOSTART, Rpi_MEASURE_MODE, RpiCmd, RpiCommands, RpiConfig, RpiEvents, RpiService,
  RpiUiEvt, Rpi_MEASURE_MODE_list, Rpi_KINETIC_MODE_list, Rpi_CALIBRATION_MODE_list, Rpi_AUTOSTART_list,
  Rpi_MEASURE_MODE_str, Rpi_KINETIC_MODE_str, Rpi_CALIBRATION_MODE_str, Rpi_AUTOSTART_str, Rpi_COMPARATION_MODE,
  Rpi_COMPARATION_MODE_str
} from "../rpi.service";
import {LenguageService} from "../lenguage.service";

@Component({
  selector: 'app-option-menu',
  templateUrl: './option-menu.component.html',
  styleUrls: ['./option-menu.component.css']
})
export class OptionMenuComponent implements OnInit {

  constructor(public rpi:RpiService,public lang:LenguageService) { }

  public menu:Menu = new Menu();

  @ViewChild('b1') fan:ElementRef;
  @ViewChild('b2') measure:ElementRef;
  @ViewChild('b3') kinetic:ElementRef;
  @ViewChild('b4') calib:ElementRef;
  @ViewChild('b5') autostart:ElementRef;
  @ViewChild('b6') language:ElementRef;
  @ViewChild('b7') b7:ElementRef;

  value:number = 0;

  input$ = new Subject<number>();

  public MenuType = MenuType;

  public Rpi_MEASURE_MODE_list;
  public Rpi_KINETIC_MODE_list;
  public Rpi_CALIBRATION_MODE_list;
  public Rpi_AUTOSTART_list;

  public Rpi_MEASURE_MODE_str;
  public Rpi_KINETIC_MODE_str;
  public Rpi_CALIBRATION_MODE_str;
  public Rpi_AUTOSTART_str;

  ngOnInit() {

    this.Rpi_MEASURE_MODE_list = Rpi_MEASURE_MODE_list;
    this.Rpi_KINETIC_MODE_list = Rpi_KINETIC_MODE_list;
    this.Rpi_CALIBRATION_MODE_list = Rpi_CALIBRATION_MODE_list;
    this.Rpi_AUTOSTART_list = Rpi_AUTOSTART_list;

    this.Rpi_MEASURE_MODE_str = Rpi_MEASURE_MODE_str;
    this.Rpi_KINETIC_MODE_str = Rpi_KINETIC_MODE_str;
    this.Rpi_CALIBRATION_MODE_str = Rpi_CALIBRATION_MODE_str;
    this.Rpi_AUTOSTART_str = Rpi_AUTOSTART_str;

    this.input$
      .auditTime(50)
      .subscribe(x => {
        this.value=x;
        //console.log(x);
        //update value
        this.rpi.writeConfig(RpiConfig.FAN_SPEED,this.value).subscribe(()=>{
        });
      });

    this.menu.Clear();
    this.menu.AddButton(this.fan,null);
    this.menu.AddButton(this.measure,null);
    this.menu.AddButton(this.kinetic,null);
    this.menu.AddButton(this.calib,null);
    this.menu.AddButton(this.autostart,null);
    this.menu.AddButton(this.language,null);
    this.menu.AddButton(this.b7,null);
    this.menu.Highlight(0);

    this.rpi.subjectEvents().subscribe((evt)=>{

      if (evt === RpiEvents.button_menu){
        this.menu.NextElem();
      }else
      if (evt === RpiEvents.button_ok){
        this.menu.ClickElem();
      }

    });



    this.rpi.readConfig(RpiConfig.FAN_SPEED).subscribe((config)=>{
        this.value = config;
    });
    this.rpi.readConfig(RpiConfig.MEASURE_MODE).subscribe((config)=>{
       this.measure.nativeElement.selectedIndex = config;
    });
    this.rpi.readConfig(RpiConfig.KINETIC_MODE).subscribe((config)=>{
      this.kinetic.nativeElement.selectedIndex = config;
    });
    this.rpi.readConfig(RpiConfig.AUTOSTART).subscribe((config)=>{
      this.autostart.nativeElement.selectedIndex = config;
    });
    this.rpi.readConfig(RpiConfig.CALIBRATION_MODE).subscribe((config)=>{
      if (config >= 128) config -= 128 - 6;
      this.calib.nativeElement.selectedIndex = config;
    });
    this.rpi.readConfig(RpiConfig.LANGUAGE).subscribe((config)=>{
      this.language.nativeElement.selectedIndex = config;
    });

      /*this.rpi.readConfig(RpiConfig.AUTOSTART).subscribe((config)=>{
        console.log(config)
      });*/



  }

  onClickSelect(ele:HTMLSelectElement,type:MenuType,evt:any){
    if (!evt.isTrusted) { //mouse event
      const size = ele.length;
      ele.selectedIndex++;
      if (ele.selectedIndex >= size || ele.selectedIndex === -1)
        ele.selectedIndex = 0;
    }

    switch(type){
      case MenuType.measure_mode:
        this.rpi.writeConfig(RpiConfig.MEASURE_MODE,+ele.value).subscribe(()=>{
        });
        break;

      case MenuType.kinetic_mode:
        this.rpi.writeConfig(RpiConfig.KINETIC_MODE,+ele.value).subscribe(()=>{
        });
        break;

      case MenuType.calibration_mode:
        this.rpi.writeConfig(RpiConfig.CALIBRATION_MODE,+ele.value).subscribe(()=>{
        });
        break;

      case MenuType.autostart:
        this.rpi.writeConfig(RpiConfig.AUTOSTART,+ele.value).subscribe(()=>{
        });
        break;

      case MenuType.language:
        this.rpi.writeConfig(RpiConfig.LANGUAGE,+ele.value).subscribe(()=>{
        });
        this.lang.changeLanguage(+ele.value);
        break;
    }
  }

  onClickSlider(evt:any){
    if (evt.isTrusted) return; //mouse

    this.value+=10;
    if (this.value > 100)
      this.value = 0;

    this.rpi.writeConfig(RpiConfig.FAN_SPEED,this.value).subscribe(()=>{
    });

  }

}

export enum MenuType{
  measure_mode,
  kinetic_mode,
  calibration_mode,
  autostart,
  language,
}

