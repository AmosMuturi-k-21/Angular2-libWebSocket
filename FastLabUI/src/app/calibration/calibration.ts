export class FastLabCalibration{


  public calibration2point(currentVal1:number,currentVal2:number,result1:number,result2:number):{delta:number,slope:number}{
      const delta = -((result2*currentVal1 - result1*currentVal2)/(result1 - result2));
      const slope = (currentVal1-currentVal2)/(result1 - result2);
      return {delta,slope};
  }

  /* aw calibration*/
  /*public calibrate_aw(aw_1:number, aw_2:number, aw_result_1:number, aw_result_2 :number):{delta:number,slope:number}{
    let slope =1;
    let delta = 0;

    slope = (aw_result_2 - aw_result_1)/(aw_2 - aw_1);
    //slope = (delta + aw_result_1)/aw_1;
    //if (slope == 0) slope

    return {delta,slope};
  }*/

  /*external calibration
  * aw_mes_x = measured value
  * aw_rech_x = measure wanted
  * tsurf_x = surface temperature
  *
  * */
  public calibrate_tmir_aw(aw_mes_1:number,aw_mes_2:number,
                           aw_rech_1:number,aw_rech_2:number,
                           tsurf_1:number,tsurf_2:number,
                           delta_measure:number,slope:number):{delta:number,slope:number}{

    if (aw_mes_1 == null || aw_mes_2 == null || aw_rech_1 == null || aw_rech_2 == null || tsurf_1 == null || tsurf_2 == null ||
      delta_measure == null || slope == null) return {delta: 0,slope: 1};

    let pas;
    let tmir_2;
    let tmir_1;
    let tmir_rech;
    let tmir_1_etal, tmir_2_etal;

    /*values to return*/
    let offset_corr = 0;
    let pente_corr = 1;

    /*@TODO check if you need the real value from the board or?!*/
    const pente_actu = slope;
    const offset_actu = delta_measure;

    tmir_2=tsurf_2;
    pas=-5.0;
    do
    {
      tmir_2+=pas;
      if ((this.fct_aw(tmir_2,tsurf_2)<aw_mes_2) && (pas<0.0)) pas/=-2.0;
      if ((this.fct_aw(tmir_2,tsurf_2)>aw_mes_2) && (pas>0.0)) pas/=-2.0;
    } while (this.AbsFl(pas)>0.001);

    tmir_1=tsurf_1;
    pas=-5.0;
    do
    {
      tmir_1+=pas;
      if ((this.fct_aw(tmir_1,tsurf_1)<aw_mes_1) && (pas<0.0)) pas/=-2.0;
      if ((this.fct_aw(tmir_1,tsurf_1)>aw_mes_1) && (pas>0.0)) pas/=-2.0;
    } while (this.AbsFl(pas)>0.001);

    tmir_2_etal=tmir_2;
    pas=1.0;
    do
    {
      tmir_2_etal+=pas;
      if ((this.fct_aw(tmir_2_etal,tsurf_2)<aw_rech_2) && (pas<0.0)) pas/=-2.0;
      if ((this.fct_aw(tmir_2_etal,tsurf_2)>aw_rech_2) && (pas>0.0)) pas/=-2.0;
    } while (this.AbsFl(pas)>0.001);

    tmir_1_etal=tmir_1;
    pas=1.0;
    do
    {
      tmir_1_etal+=pas;
      if ((this.fct_aw(tmir_1_etal,tsurf_1)<aw_rech_1) && (pas<0.0)) pas/=-2.0;
      if ((this.fct_aw(tmir_1_etal,tsurf_1)>aw_rech_1) && (pas>0.0)) pas/=-2.0;
    } while (this.AbsFl(pas)>0.001);

    // if (abs_fl(tmir_2_etal-tmir_1_etal)<4.0) message3("ATTENTION POINTS D'ETALONNAGE PROCHES");

    pente_corr=pente_actu*(tmir_1_etal-tmir_2_etal)/(tmir_1-tmir_2);

    pas=1.0;
    offset_corr=0.0;
    do
    {
      offset_corr+=pas;
      tmir_rech=pente_corr/pente_actu*(tmir_2-offset_actu)+offset_corr;

      if ((tmir_rech<tmir_2_etal) && (pas<0.0)) pas/=-2.0;
      if ((tmir_rech>tmir_2_etal) && (pas>0.0)) pas/=-2.0;

    } while (this.AbsFl(pas)>0.001);

    return {delta:offset_corr,slope:pente_corr};
  }

  public calibrate_tsurf_aw(aw_mes_1:number,aw_mes_2:number,
                            aw_rech_1:number,aw_rech_2:number,
                            tsurf_1:number,tsurf_2:number,
                            delta_measure:number,slope_measure:number):{delta:number,slope:number} {

    if (aw_mes_1 == null || aw_mes_2 == null || aw_rech_1 == null || aw_rech_2 == null || tsurf_1 == null || tsurf_2 == null ||
      delta_measure == null || slope_measure == null) return {delta: 0,slope: 1};

    let pas;
    let tmir_2;
    let tmir_1;
    let tsurf_rech;
    let tsurf_1_etal,tsurf_2_etal;

    /*values to return*/
    let offset_corr = 0;
    let pente_corr = 1;

    /*@TODO check if you need the real value from the board or?!*/
    const pente_actu = slope_measure;
    const offset_actu = delta_measure;

    console.log(aw_mes_1,aw_mes_2,aw_rech_1,aw_rech_2,tsurf_1,tsurf_2,delta_measure,slope_measure);

    tmir_2=tsurf_2;
    pas=-5.0;
    do
    {
      tmir_2+=pas;
      if ((this.fct_aw(tmir_2,tsurf_2)<aw_mes_2) && (pas<0.0)) pas/=-2.0;
      if ((this.fct_aw(tmir_2,tsurf_2)>aw_mes_2) && (pas>0.0)) pas/=-2.0;
    } while (this.AbsFl(pas)>0.001);

    tmir_1=tsurf_1;
    pas=-5.0;
    do
    {
      tmir_1+=pas;
      if ((this.fct_aw(tmir_1,tsurf_1)<aw_mes_1) && (pas<0.0)) pas/=-2.0;
      if ((this.fct_aw(tmir_1,tsurf_1)>aw_mes_1) && (pas>0.0)) pas/=-2.0;
    } while (this.AbsFl(pas)>0.001);

    tsurf_2_etal=tsurf_2;
    pas=1.0;
    do
    {
      tsurf_2_etal+=pas;
      if ((this.fct_aw(tmir_2,tsurf_2_etal)>aw_rech_2) && (pas<0.0)) pas/=-2.0;
      if ((this.fct_aw(tmir_2,tsurf_2_etal)<aw_rech_2) && (pas>0.0)) pas/=-2.0;
    } while (this.AbsFl(pas)>0.001);

    tsurf_1_etal=tsurf_1;
    pas=1.0;
    do
    {
      tsurf_1_etal+=pas;
      if ((this.fct_aw(tmir_1,tsurf_1_etal)>aw_rech_1) && (pas<0.0)) pas/=-2.0;
      if ((this.fct_aw(tmir_1,tsurf_1_etal)<aw_rech_1) && (pas>0.0)) pas/=-2.0;
    } while (this.AbsFl(pas)>0.001);

    // if (abs_fl(tsurf_2_etal-tsurf_1_etal)<2.0) message3("ATTENTION POINTS D'ETALONNAGE PROCHES");

    //was
    pente_corr=pente_actu*(tsurf_1_etal-tsurf_2_etal)/(tsurf_1-tsurf_2);

    //fixed
    //pente_corr=pente_actu*(tsurf_1_etal-tsurf_2_etal)/(tsurf_1-tsurf_2);

    pas=1.0;
    offset_corr=0.0;
    do
    {
      offset_corr+=pas;
      tsurf_rech=pente_corr/pente_actu*(tsurf_2-offset_actu)+offset_corr;

      if ((tsurf_rech<tsurf_2_etal) && (pas<0.0)) pas/=-2.0;
      if ((tsurf_rech>tsurf_2_etal) && (pas>0.0)) pas/=-2.0;

    } while (this.AbsFl(pas)>0.001);

    return {delta:offset_corr,slope:pente_corr};
  }

  public AbsFl(A:number){
    return (A)>0.0?(A):(-(A));
  }

  public fct_aw( miroir:number,  surface:number):number
  {
  let aw;

  miroir+=273.15;
  surface+=273.15;

  aw=-6096.9385*(1.0/miroir-1.0/surface);
  aw-=0.02711193*(miroir-surface);
  aw+=0.00001673952*((miroir*miroir)-(surface*surface));
  aw+=2.433502*Math.log(miroir/surface);
  aw=Math.exp(aw);

  return(aw);
  }
}
