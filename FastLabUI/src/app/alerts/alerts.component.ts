import { Component, OnInit,AfterViewInit, Input , Output, EventEmitter, ViewChild, ElementRef, ViewChildren, QueryList,
  OnDestroy,
  AfterContentInit,
  OnChanges } from '@angular/core';
import {RpiService,RpiCommands, RpiEvents,} from "../rpi.service";
import {Menu} from "../menu";
import {LenguageService} from "../lenguage.service";
import {Subscription} from "rxjs/Subscription";

@Component({
  selector: 'app-alerts',
  templateUrl: './alerts.component.html',
  styleUrls: ['./alerts.component.css']
})
export class AlertsComponent implements OnInit ,AfterViewInit, OnDestroy,AfterContentInit, OnChanges {
  public menu:Menu = new Menu("pop-up");
  // @ViewChild('bok') bok:ElementRef;
  @ViewChild('bcancel') bcancel:ElementRef;
  @ViewChild('bcancel2') bcancel2:ElementRef;
  @ViewChild('b1') b1:ElementRef;
  @ViewChild('b2') b2:ElementRef;
  @ViewChildren('saltRef') saltRef:QueryList<ElementRef>;
  currentButton:number = 0;

  eventSub:Subscription;

  saltArray = [
    "LiCl",
    "MgCl2",
    "K2CO3",
    "MgNO3",
    "NaCl",
    "KCl",
    "K2SO4",
    "H2O",
    "0.100",
    "0.125",
    "0.200",
    "0.250",
    "0.300",
    "0.340",
    "0.350",
    "0.400",
    "0.500",
    "0.530",
    "0.600",
    "0.650",
    "0.700",
    "0.760",
    "0.800",
    "0.900",
    "0.950",
    "0.980",
    "1.000"
  ];

  saltValueArray=[
    113,
    331,
    432,
    544,
    755,
    851,
    976,
    1000,
    100,
    125,
    200,
    250,
    300,
    340,
    350,
    400,
    500,
    530,
    600,
    650,
    700,
    760,
    800,
    900,
    950,
    980,
    1000,
  ];

  saltPage=[];

  sPage = 0;
  sNum = 9;


  @Input() public alertMsg:string;
  //@Input()  alert:boolean = true;
  @Input() public alertBtn = false;
  @Input() public salts = false;
  @Output() public ok:EventEmitter<MouseEvent> = new EventEmitter<MouseEvent>();
  @Output()  public cancel:EventEmitter<MouseEvent> = new EventEmitter<MouseEvent>();
  @Output() public salt:any = new EventEmitter();

  constructor(public rpi:RpiService,public lang:LenguageService) { }


  onOk(){
    this.ok.emit();
  }
  onCancel(){
    this.cancel.emit();
  }

  onSalt(name,i) {
    this.salt.emit({index:i,val:this.saltValueArray[i-1]});
  }

  ngOnDestroy() {
    console.log("pop-up destroyed");
    this.menu.Clear();
    if (this.eventSub != null) {
      this.eventSub.unsubscribe();
      this.eventSub = null;
    }
  }

  ngOnChanges(obj){
    this.getMenu(false);
    this.menu.SetCurrentButton(this.currentButton);
    //this.menu.SetCurrentButton(0);
  }

  ngAfterViewInit(){
    this.getMenu(true);
    this.menu.SetCurrentButton(this.currentButton);
  }
  ngAfterContentInit(){

  }

  ngOnInit() {
    console.log("pop-up created");
    console.log(this.menu);
      this.currentButton = 0;
      this.sPage = 0;
      this.getSalt();

    this.eventSub = this.rpi.subjectEvents().subscribe((evt) => {

        if (this.alertBtn || this.salts) {
          switch (evt) {
            case RpiEvents.button_menu:
              this.menu.NextElem();
              break;

            case RpiEvents.button_ok:
              this.menu.ClickElem();
              break;
          }
        }
      });
  }

  getSalt() {
   this.saltPage = this.saltArray.slice(this.sNum*this.sPage,this.sPage*this.sNum+this.sNum);
    if (this.saltRef != null)
      this.saltRef.notifyOnChanges();
  }

  getMenu(init){
    if (this.saltRef == null) return;
    this.menu.Clear();
    if(this.alertBtn || this.salts) {
      //setTimeout(()=>{
        this.menu.AddButton(this.b1,null);
        this.menu.AddButton(this.b2,null);
        if(this.salts){
          this.menu.AddButton(this.bcancel2, null);
        }
        if(this.alertBtn){
          this.menu.AddButton(this.bcancel, null);
        }

      if (this.saltRef.length === 0) {
        console.error("saltref empty");
        this.getSalt();
        setTimeout(()=>{this.getMenu(false);},50);
        return;
      }

        this.saltRef.forEach((val:any) => {
            this.menu.AddButton(val,null);
        });

        if(init === true) {
          this.menu.Highlight(0);
        }
        console.log(this.menu);
        this.menu.NextElem();
      //},0);
      // this.menu.AddButton(this.bok, null);
    }
  }

  onPageUp() {
    this.sPage++;
    this.currentButton = this.menu.GetCurrentButton();
    if (this.sPage * this.sNum >= this.saltArray.length){
      this.sPage--;
    }
    this.getSalt();
    setTimeout(()=>{this.getMenu(false);},50);

  };

  onPageDown() {
    this.sPage--;
    this.currentButton = this.menu.GetCurrentButton();
    if (this.sPage <= 0) this.sPage = 0;
    this.getSalt();
    setTimeout(()=>{this.getMenu(false);},50);

  };

}




