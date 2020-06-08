import { Directive, HostBinding, HostListener, OnInit, DoCheck,  } from '@angular/core';
import {RpiEvents, RpiService} from "./rpi.service";
import {Router} from "@angular/router";

@Directive({
  selector: '[appStartBtn]'
})
export class StartBtnDirective implements OnInit , DoCheck{
  constructor(public rpi:RpiService, private router:Router){}



  @HostBinding('class.stop') stop = false;
  @HostListener('click') stopped() {this.stop = !this.stop}


ngOnInit(){
   /* if(this.router.url.includes('calibration')) {
      if(this.rpi.rpiData.measure_started === 1){this.stop = false;}
      if (this.rpi.rpiData.measure_started === 0){this.stop = true;}
}; */

    if(this.rpi.rpiData.measure_started === 0){this.stop = false;}
    if (this.rpi.rpiData.measure_started === 1){this.stop = true;}}


ngDoCheck() {if(this.rpi.rpiData.measure_started === 0){this.stop = false;}
  if (this.rpi.rpiData.measure_started === 1){this.stop = true;}}
}


