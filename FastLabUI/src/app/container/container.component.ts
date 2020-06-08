import {Component, DoCheck, ElementRef, HostListener, OnInit, ViewChild} from '@angular/core';
import {RpiConfig, RpiService} from "../rpi.service";
import {environment} from "../../environments/environment";
import {LenguageService} from "../lenguage.service";
import {Router} from "@angular/router";
import {Subscription} from "rxjs/Subscription";

@Component({
  selector: 'app-container',
  templateUrl: './container.component.html',
  styleUrls: ['./container.component.css']
})
export class ContainerComponent implements OnInit, DoCheck {
english=[];
  initialized = false;
  hideLogo = false;
  eventSub:Subscription = null;
  connected:boolean;

  showConsole = false;
  @ViewChild('console') console:ElementRef;

  public KeyList = '';

  constructor(private rpi:RpiService, private lang:LenguageService,public router: Router) {
    this.rpi.ConnectionStatusObservable.subscribe((val)=>{
      this.connected = val
    })
  }

  ngOnInit() {
    this.boot_func();
    document.body.addEventListener('keydown',(ev)=>{
      //console.log(ev);
      if(ev.key === 'a') {
        this.rpi.buttons.press('button_ok');
      }
      if(ev.key === 's') {
        this.rpi.buttons.press('button_menu')
      }

    })
  }

  ngDoCheck() {};

  boot_func(){
    console.log("production = %s",environment.production );
    if (environment.production)
      this.rpi.init(window.location.host);

    else
      this.rpi.init("192.168.2.81:3000");
    //console.log(window.location);

    setTimeout(()=>{
      this.hideLogo = true;
    },1200);

    this.eventSub = this.rpi.readConfig(RpiConfig.LANGUAGE).subscribe((num)=>{
      this.lang.initialize(num);

      /*setTimeout(()=>{
        this.initialized = true;
        if (this.eventSub != null){
          this.eventSub.unsubscribe();
          this.eventSub = null;
        }
      },500);*/

      this.initialized = true;
      if (this.eventSub != null){
        this.eventSub.unsubscribe();
        this.eventSub = null;
      }

      //this.router.navigateByUrl("/menu");

      this.router.navigateByUrl("/menu");

    });


    setTimeout(()=>{
      if (this.initialized === false){
        if (this.eventSub != null){
          this.eventSub.unsubscribe();
          this.eventSub = null;
        }
        this.boot_func();
      }
    },2000);
  }

  @HostListener('window:keydown', ['$event'])
  keyboardInput(event: KeyboardEvent) {

    if (event.key.length === 1) {

      //clean by error
      if (this.KeyList === 'Error' || this.KeyList === 'Ok'){
        this.KeyList = '';
      }

      this.KeyList += event.key;
    }

    //console.log(event);

    if (event.key === '~'){
      this.KeyList = '>';
      this.showConsole = !this.showConsole;
    }else
    if (event.key === 'Backspace'){
      this.KeyList = this.KeyList.substring(0, this.KeyList.length-1);
    }else
      if (event.key === 'Enter'){ //execute command

        if (this.KeyList === 'Error' || this.KeyList === 'Ok'){
           return; //skip this
        }

        const value = this.ConsoleParser(this.KeyList);

        console.log(value);

        switch (value[0].key){
          case "service":
            if (this.ConsoleNull(value[0]))
              this.rpi.devMode = !this.rpi.devMode;
            else
              this.rpi.devMode = this.ConsoleBool(value[0]);
            break;

          case "production":
            if (this.ConsoleNull(value[0]))
              environment.production = !environment.production;
            else
              environment.production = this.ConsoleBool(value[0]);
            break;

          case "debug":
            if (this.ConsoleNull(value[0]))
              environment.enable_debug = !environment.enable_debug;
            else
              environment.enable_debug = this.ConsoleBool(value[0]);
            break;

          default:
            this.KeyList = 'Error';
        }

        if (this.KeyList !== 'Error') {
            this.KeyList = 'Ok';
            setTimeout(()=>{
              this.showConsole = false; //hide
            },1000);
        }
    }
  }

  ConsoleParser(parse:string):{key:string,value:string}[]{
     if (parse.charAt(0) === '>')
       parse = parse.substr(1,parse.length-1);

      parse = parse.trim().toLowerCase();

      let index = parse.indexOf('=');
      if (index === -1){
      index = parse.indexOf(':');
    }
    if (index === -1){
      index = parse.indexOf(' ');
    }

    let key = '';
    let value = null;

    if (index >= 0)
      key = parse.substr(0,index).trim();
    else
      key = parse;

    if (index >= 0)
      value = parse.substr(index+1,parse.length).trim();

      return [{key:key,value:value}];
  }

  ConsoleNull(val:{key:string,value:string}):boolean{
    if (val.value == null || val.value === '') return true;
    return false;
  }
  ConsoleBool(val:{key:string,value:string}):boolean{
    if (val.value == null || val.value === '') return false;
    if (val.value === 'true') return true;
    if (val.value === '1') return true;
    return false;
  }


}
