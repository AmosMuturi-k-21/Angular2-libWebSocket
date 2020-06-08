import { Component, OnInit, OnChanges, DoCheck, ViewChild, ElementRef, OnDestroy } from '@angular/core';
import {RpiCommands, RpiEvents, RpiMeasureData, RpiService} from "../rpi.service";
import {Menu} from "../menu";
import {Subscription} from "rxjs/Subscription";
import {LenguageService} from "../lenguage.service";
import {forEach} from "@angular/router/src/utils/collection";

@Component({
  selector: 'app-list-menu',
  templateUrl: './list-menu.component.html',
  styleUrls: ['./list-menu.component.css']
})
export class ListMenuComponent implements OnInit , OnDestroy{
  public menu:Menu = new Menu();

  @ViewChild('b1') b1:ElementRef;
  @ViewChild('b2') b2:ElementRef;
  @ViewChild('b3') b3:ElementRef;
  @ViewChild('b4') b4:ElementRef;

  //sliceStart = 10;
  page = 0;
  pageSize = 7; //was 9
  isready = false;
  pageNumber = 0;
  pages = [0,1,2];
  lista:RpiMeasureData[] = [];
  constructor(public rpi:RpiService,public lang:LenguageService) { }

  getPage():RpiMeasureData[]{
    if (!this.isready) return [];
    if (this.page * this.pageSize >= this.lista.length)return this.lista.slice(this.lista.length - this.pageSize,this.lista.length);
     return  this.lista.slice(this.page * this.pageSize,this.page * this.pageSize + this.pageSize); //0 -> 10 (0 - 10) , 1- > 10,10 (10-20)
  }

  onPageUp() {
    this.page++;
    if (this.page * this.pageSize > this.lista.length){
       this.page--;
    }
    };

  onPageDown() {
    this.page--;
    if (this.page <= 0) this.page = 0;
    };


  private events:Subscription = null;
  private Dataevents:Subscription = null;
  private Uievents:Subscription = null;

  ngOnDestroy(){
    if (this.events) this.events.unsubscribe();
    if (this.Dataevents) this.Dataevents.unsubscribe();
    if (this.Uievents) this.Uievents.unsubscribe();
  }


  updateMeasureList(){
    this.rpi.cmd(RpiCommands.history).subscribe((data)=>{
      this.lista = [];

      /* remove empty measure*/
      for(let i = 0;i<data.response.length;i++){
        if (data.response[i].date !== 0)
          this.lista.push(data.response[i]);
      }

      /* sort the list */
      this.lista = this.lista.sort(function (a,b) {
        if(a.date < b.date) {
          return 1
        }
        if(a.date > b.date){
          return -1
        }
      });

      /* truncate the list to 25*/
      if (this.lista.length > 25)
          this.lista = this.lista.slice(0, 25);


      this.pageNumber = Math.ceil(this.lista.length / this.pageSize);
      this.isready = true;
    });
  }

  ngOnInit() {
  this.updateMeasureList();

    this.rpi.subjectEvents().subscribe((evt)=>{
      if(evt === RpiEvents.measure_end) {
        this.updateMeasureList();
      }
    });

    this.menu.Clear();
    this.menu.AddButton(this.b1,null);
    this.menu.AddButton(this.b2,null);
    this.menu.AddButton(this.b3,null);
    this.menu.AddButton(this.b4,null);
    this.menu.Highlight(0);

   this.events = this.rpi.subjectEvents().subscribe((evt)=>{

      if (evt === RpiEvents.button_menu){
        this.menu.NextElem();
      }else
      if (evt === RpiEvents.button_ok){
        this.menu.ClickElem();
      }

    });


  }

}




