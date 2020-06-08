import { Injectable } from '@angular/core';

@Injectable()
export class StorageService {

  constructor() { }

  public alertMsg:string;
  public alert:boolean;
  public sampleAw = "Aw";
  public sampleName= "----";
  public sampleComp:string;
  public sampleToggle= 0;
  public calibStartedYet = false;
  public xiLimit = 50;
  public calibAw = 0;
  public calibAw2 = 0;
  public sampleSaltValue = 0;
  public sampleSaltValue2:number = 0;
}
