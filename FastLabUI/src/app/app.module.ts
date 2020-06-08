import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { MainMenuComponent } from './main-menu/main-menu.component';
import { ListMenuComponent } from './list-menu/list-menu.component';
import { CalibrationMenuComponent } from './calibration-menu/calibration-menu.component';
import { OptionMenuComponent } from './option-menu/option-menu.component';
import {Routes , RouterModule} from "@angular/router";
import { MesurementMenuComponent } from './mesurement-menu/mesurement-menu.component';
import { ContainerComponent } from './container/container.component';
import {RpiService} from "./rpi.service";
import { AlertsComponent } from './alerts/alerts.component';
import { StartBtnDirective } from './start-btn.directive';

import {BrowserAnimationsModule} from '@angular/platform-browser/animations';
import 'hammerjs';
import {LenguageService} from "./lenguage.service";
import {HttpModule} from "@angular/http";
import { DebugComponent } from './debug/debug.component';
import { AdvancedOptionMenuComponent } from './advanced-option-menu/advanced-option-menu.component';
import {StorageService} from "./storage.service";
import { SecondsToTimePipe } from './seconds-to-time.pipe';
import {ChartsModule} from "ng2-charts";
import {MatSlider, MatSliderModule} from "@angular/material";
import {FormsModule, ReactiveFormsModule} from "@angular/forms";


const pagine: Routes = [
  {path: "", redirectTo: "", pathMatch: "full"},
  {path:'menu', component:MainMenuComponent},
  {path:'mesurement', component:MesurementMenuComponent},
  {path: 'list', component: ListMenuComponent},
  {path: 'calibration', component: CalibrationMenuComponent},
  {path: 'options', component: OptionMenuComponent},
  {path: 'debug', component: DebugComponent},
  {path: "advanced", component: AdvancedOptionMenuComponent},

];

@NgModule({
  declarations: [
    AppComponent,
    MainMenuComponent,
    ListMenuComponent,
    CalibrationMenuComponent,
    OptionMenuComponent,
    MesurementMenuComponent,
    ContainerComponent,
    AlertsComponent,
    StartBtnDirective,
    DebugComponent,
    AdvancedOptionMenuComponent,
    SecondsToTimePipe,


  ],
  imports: [
    BrowserModule,
    RouterModule.forRoot(pagine),
    MatSliderModule,
    BrowserAnimationsModule,
    HttpModule,
    ChartsModule,
    FormsModule,
    ReactiveFormsModule,
  ],

  exports: [],

  providers: [
    RpiService,
    LenguageService,
    StorageService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }

