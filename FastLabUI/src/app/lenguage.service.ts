import { Injectable } from '@angular/core';
import { Http } from  '@angular/http';


@Injectable()
export class LenguageService {

  public LanguageList = [];
  private LanguageFile:any[] = null;
  public Language:LanguageInterface  = {
    "name":"English",

    "main_menu_btn_mesure": "Measurement",
    "main_menu_btn_list": "Measurement list",
    "main_menu_btn_calibration": "Calibration",
    "main_menu_btn_options": "Options",
    "main_menu_btn_advance": "Advanced options",


    "start": "START",
    "stop": "STOP",
    "btn_menu": "Menu",
    "alert_head_down":"Error. Please pull down the measurement head. Operation aborted.",
    "alert_start": "Measurement started",
    "alert_stop":"Operation aborted by user.",
    "alert_title":"Event log",
    "data_surface":"Surface temperature",
    "data_sensor":"Opt. Sensor",
    "data_mirror":"Mirror",
    "data_time":"Time",
    "data_peltier":"Peltier",
    "data_aw":"Aw",
    "data_waterA":"WATER ACTIVITY",
    "data_now":"Now",
    "data_max":"Max",
    "data_dif":"Dif",
    "data_min":"Min",
    "data_null":"NaN",
    "graph_title":"Surface temperature",
    "graph_title_obj":"Object temperature",


    "list_aw":"Aw",
    "list_date":"Date",
    "list_temperature":"Surface temperature",
    "list_name":"Name",
    "btn_back":"Back",
    "btn_nest":"Next",


    "calib_aw_M":"Aw measure",
    "calib_aw_R":"Aw reach",
    "calib_slope":"Slope",
    "calib_delta":"Delta",

    "calib_txt_off":"Press start to initialize the calibration procedure.",
    "calib_txt_on":"Calibrating...",
    "calib_txt_aw_calc":"AW calculated.",
    "calib_txt_aw_salt":"AW salt sample",
    "calib_txt_warn":"Warning measurement running, starting a calibration will abort the measurement.",
    "alert_cal_started":"Calibration started.",
    "alert_salt_1":"Please put the GBX salt in the sample zone and press 'OK'.",
    "alert_salt_2":"Please put the second GBX salt in the sample zone and press 'OK'.",
    "alert_err_salt":"Error. salt used are of the same type. Operation aborted.",
    "alert_err_closeSalt":"Error. Salt used are too similar. Operation aborted.",
    "alert_err":"Error. Operation aborted.",
    "alert_err_mirror":"Please clean the mesurement mirror. Operation aborted.",
    "alert_cal_end":"Calibration succesfully ended.",
    "alert_measure_end":"Measurement succesfully ended.",
    "alert_measure_end_fan":"Measurement ended, fan still running.",
    "alert_measure_error":"Measurement error: please clean the mirror.",
    "alert_measure_end_no_fan":"Measurement and Fan stopped ",
    "alert_btn_ok":"Ok",
    "alert_btn_cancel":"Cancel",


    "opt_fan_speed":"Fan speed",
    "opt_mode":"Measurement mode",
    "opt_kinetic":"Kinetic mode",
    "opt_calibration":"Calibration mode",
    "opt_autoStart":"Auto start",
    "opt_lenguage":"Language",

    "opt_select_measure" : [
      "2_5m",
      "3_10m",
      "6_30m",
      "fix_10_m",
      "2_5_flash",
      "3_10_flash",
      "6_30_flash",
      "10_flash",
    ],
    "opt_select_kinetic":[
      "15m_2_30",
      "30m_2_30",
      "1h_15",
      "2h_10",
      "4h_15",
      "8h_30",
      "cont_60",
      "cont_2_30"
    ],
    "opt_select_calib":[
      "1p_1l",
      "1p_2l",
      "1p_3l",
      "1p_4l",
      "1p_5l",
      "1p_6l",
      "2p_1l",
      "2p_2l",
      "2p_3l",
      "2p_4l",
      "2p_5l",
      "2p_6l"
    ],
    "opt_select_autostart":[
      "off",
      "measure_1",
      "measure_2",
      "kinetic_1",
      "kinetic_2"
    ],
    "ad_opt_sample_temp":"Sample temperature",
    "ad_opt_sample":"Sample presets",
    "ad_opt_name":"Sample name",
    "ad_opt_aw":"Sample AW",
    "ad_opt_mode":"Comparation",
    "ad_opt_info":"Here you can create some custom presets,to be then selected in the measurement menu and compared with the live measure. Choose the name,value and if the measure must likely be miron or major to the preset."

  };

  constructor(private http: Http) {}

  /*init with index = x*/
  initialize(lang:number = 0){
     //return this.http.get("",lenguage);
      this.http.get("/assets/doc/language.txt").subscribe((file)=>{

          try{
              const dat:any[] = file.json();
              if (dat == null){
                console.error("parsing json error");
                return;
              }
              this.LanguageFile = dat;

              for(let ll of dat){
                  this.LanguageList.push(ll.name);
              }

              this.changeLanguage(lang);

          }catch (err){
              console.error(err);
          }

      },(err)=>{
        console.error(err);
      });
  }

  public changeLanguage(lang:number = 0){
    if (this.LanguageFile == null) return;
    if (lang < this.LanguageFile.length){
      this.Language = this.LanguageFile[lang];
      console.log("language loaded [%s]",this.Language.name);
    }else {
      console.error("language index out of range [>%s]", this.LanguageFile.length);
      this.Language = this.LanguageFile[0];
    }
  }



}


export interface LanguageInterface{
  "name":string,

  "main_menu_btn_mesure": string,
  "main_menu_btn_list": string,
  "main_menu_btn_calibration":string,
  "main_menu_btn_options": string,
  "main_menu_btn_advance": string,

  "start": string,
  "stop": string,
  "btn_menu": string,
  "alert_head_down":string,
  "alert_start": string,
  "alert_stop":string,
  "alert_title":string,
  "data_surface":string,
  "data_sensor":string,
  "data_mirror":string,
  "data_time":string,
  "data_peltier":string,
  "data_aw":string,
  "data_waterA":string,
  "data_now":string,
  "data_max":string,
  "data_dif":string,
  "data_min":string,
  "data_null":string,
  "graph_title":string,
  "graph_title_obj":string,

  "list_aw":string,
  "list_date":string,
  "list_temperature":string,
  "list_name":string,
  "btn_back":string,
  "btn_nest":string,

  "calib_aw_M":string
  "calib_aw_R":string
  "calib_slope":string
  "calib_delta":string

  "calib_txt_off":string,
  "calib_txt_on":string,
  "calib_txt_warn":string,
  "calib_txt_aw_calc":string,
  "calib_txt_aw_salt":string,
  "alert_cal_started":string,
  "alert_salt_1":string,
  "alert_salt_2":string,
  "alert_err_salt":string,
  "alert_err_closeSalt":string,
  "alert_err":string,
  "alert_err_mirror":string,
  "alert_cal_end":string,
  "alert_measure_end":string,
  "alert_measure_end_fan":string,
  "alert_measure_error":string,
  "alert_measure_end_no_fan":string,
  "alert_btn_ok":string,
  "alert_btn_cancel":string,

  "opt_fan_speed":string,
  "opt_mode":string,
  "opt_kinetic":string,
  "opt_calibration":string,
  "opt_autoStart":string,
  "opt_lenguage":string,

  "opt_select_measure" :string[
    ],
  "opt_select_kinetic":string[
    ],
  "opt_select_calib":string[
    ],
  "opt_select_autostart":string[
    ],

  "ad_opt_sample":string,
  "ad_opt_sample_temp":string,
  "ad_opt_name":string,
  "ad_opt_aw":string,
  "ad_opt_mode":string,
  "ad_opt_info":string
}


