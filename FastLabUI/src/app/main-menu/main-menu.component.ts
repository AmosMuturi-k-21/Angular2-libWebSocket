import {Component, OnInit, ViewChild, ElementRef, OnDestroy, HostListener} from '@angular/core';
import {RpiEvents, RpiService} from "../rpi.service";
import {Menu} from "../menu";
import {Subject} from "rxjs/Subject";
import {Subscription} from "rxjs/Subscription";
import {LenguageService} from "../lenguage.service";
import {environment} from "../../environments/environment";

@Component({
  selector: 'app-main-menu',
  templateUrl: './main-menu.component.html',
  styleUrls: ['./main-menu.component.css']
})
export class MainMenuComponent implements OnInit,OnDestroy {

  public menu:Menu = new Menu();

  @ViewChild('b1') b1:ElementRef;
  @ViewChild('b2') b2:ElementRef;
  @ViewChild('b3') b3:ElementRef;
  @ViewChild('b4') b4:ElementRef;
  @ViewChild('b4') b5:ElementRef;
  @ViewChild('bdebug') bdebug:ElementRef;

  constructor(public rpi:RpiService,public lang:LenguageService) { }

  private event:Subscription;

  ngOnDestroy(){
      if (this.event) this.event.unsubscribe();

  }

  ngOnInit() {
    this.menu.Clear();
    this.menu.AddButton(this.b1,null);
    this.menu.AddButton(this.b2,null);
    this.menu.AddButton(this.b3,null);
    this.menu.AddButton(this.b4,null);
    this.menu.AddButton(this.b5,null);

    if (this.rpi.devMode)
      this.menu.AddButton(this.bdebug,null);

    this.menu.AddButton(this.b4,null);

    this.menu.Highlight(0);


    this.event=this.rpi.subjectEvents().subscribe((evt)=>{

      if (evt === RpiEvents.button_menu){
        this.menu.NextElem();
      }else
      if (evt === RpiEvents.button_ok){
        this.menu.ClickElem();
      }

    });
  }

  hideMenu():boolean{
    return !environment.enable_debug;
  }

}
