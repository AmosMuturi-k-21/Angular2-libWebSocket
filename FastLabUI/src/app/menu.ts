import { ElementRef } from '@angular/core';

export class Menu {

  private id =0;
  private list:{code:Function,href:ElementRef,id:number}[] = [];
  private index = 0;
  private inactive = false;
  private subMenu:Menu = null;
  private mainMenu:Menu = null;
  //private activeDate = Date.now();
  constructor(public name:string = "..."){
    this.Clear();
  }

  public OpenSubMenu(menu:Menu){
      this.SubMenuClose(); //close any submenu opened

      /* deactive this menu */
      this.setActive(false);

      /*open new menu*/
      this.subMenu = menu;
      this.subMenu.setActive(true);
      this.subMenu.setMainMenu(this);
  }

  public setMainMenu(menu:Menu){
    this.mainMenu = menu;
  }

  public SubMenuClose(){
    if (this.subMenu == null) return;
    this.subMenu.setActive(false);
    this.setActive(true);
    this.subMenu = null;
  }

  public Clear(){
  this.id =0;
  this.list = [];
  this.index = 0;
  this.inactive = false;

  /*setTimeout(()=>{
    this.inactive = false;
  },200);*/
  }

  public Close(){
    this.Clear();
    this.SubMenuClose();
    if (this.mainMenu != null){
      this.mainMenu.SubMenuClose();
    }
    this.mainMenu = null;
  }

  public setActive(value:boolean = !this.inactive) {
    if (value === false)
    this.inactive = !value;
    else
      this.inactive = !value;
      /*setTimeout(()=>{ //reactive on xxx ms
        this.inactive = !value;
      },200);*/
  }

  public AddButton(href:ElementRef,callback:Function):number{

    this.list.push({
      code:callback,
      href:href,
      id:this.id
    });
    return this.id++;
  }

  public Highlight(id:number = -1){
    if (this.inactive === true || this.list.length === 0){console.log("Menu %s [Highlight] ignored",this.name); return;};
    /*if (this.inactive === true){return}*/
    if (id <= -1) id = this.index;
    const elem = this.list[id];

    if (elem.href != null){
      if (elem.href.nativeElement != null)
      elem.href.nativeElement.focus();
    }
  }

  public SetCurrentButton(id:number){
    this.index = id;
  };

  public GetCurrentButton(){
    return this.index;
  };

  public NextElem(){
    if (this.inactive === true || this.list.length === 0){console.log("Menu %s [NextElem] ignored",this.name); return;};
    this.index++;
    if (this.index >= this.list.length){
      this.index = 0;
    }
    this.Highlight(this.index);
  }

  public ClickElem(native:boolean = true){
    this.Click(this.index);
  }

  public Click(id:number,native:boolean = true){
    if (this.inactive === true || this.list.length === 0){console.log("Menu %s [Click] ignored",name);return;};
      const elem = this.list[id];

      if (elem.href != null && native){
        if (elem.href.nativeElement != null)
        elem.href.nativeElement.click();
      }

    if (elem.code != null){
      elem.code()
    }
  }
}
