#include "string.h"

//#include "Flex333.h"
#include "proto.h"



char * float_ascii(xdata float val,xdata char nbdecimale)

//	ENTREES		:	float valeur, char nbdecimale
// SORTIES		:	buffer_ascii
// DESCRIPTION	:	converti une valeur float en chaine de caracteres

{
	//printf("buffer_ascii = [%s]\n", buffer_ascii);

	char format[] = "%.0f";
	format[2] = '0' + nbdecimale;
	int len = sprintf(&buffer_ascii[ptr_ascii], format,val);
	ptr_ascii += len;

	//printf("buffer_ascii = [%s]\n", buffer_ascii);
	/*xdata char j;
	xdata char k;


   buffer_tx.fl=val;

	#define signexp (*(buffer_tx.uc+3)&64)
	#define exposant (*(buffer_tx.uc+3)&63)
	#define signe (*(buffer_tx.uc+3)&128)
	   
   if(signe!=0)   //+ et - du nombre
		buffer_ascii[ptr_ascii++]='-';

   if((signexp!=0) && (exposant!=0)) //exposant positif
   {
      for(j=0;j<exposant;j++)
      {
			if(j%2==0) buffer_ascii[ptr_ascii++]=((*(buffer_tx.uc+j/2)&240)/16)+'0';
   		else buffer_ascii[ptr_ascii++]=(*(buffer_tx.uc+j/2)&15)+'0';
      }
   }
   else 
   {//exposant negatif ou nul
      j=0;
      buffer_ascii[ptr_ascii++]='0';
   } 

   if(nbdecimale!=0)
   {
      buffer_ascii[ptr_ascii++]=','; //on met la virgule

      for(k=0;k<nbdecimale;k=k+1)
      {
         if(signexp != 0)
         {
				if(j%2==0) buffer_ascii[ptr_ascii++]=((*(buffer_tx.uc+j/2)&240)/16)+'0';
   			else buffer_ascii[ptr_ascii++]=(*(buffer_tx.uc+j/2)&15)+'0';
            j++;      
         }
         else //exposant negatif
         {
            if((64-exposant)>k)  //on affiche le nombre de zéro nécessaires
              buffer_ascii[ptr_ascii++]='0';
            else
            {
					if(j%2==0) buffer_ascii[ptr_ascii++]=((*(buffer_tx.uc+j/2)&240)/16)+'0';
   				else buffer_ascii[ptr_ascii++]=(*(buffer_tx.uc+j/2)&15)+'0';            	
   	         j++; 
            }
         }

      }
	}*/
	buffer_ascii[ptr_ascii]=0;
	//printf("buffer_ascii = [%s]\n", buffer_ascii);
	return(buffer_ascii);
}





char * date_ascii (xdata unsigned long date)
{
   xdata unsigned long temp;
	//char temp;
//	if(date!=VIDE)
//	{
		temp=(date/86400)%32;
		buffer_ascii[ptr_ascii++]=temp/10+'0';
		buffer_ascii[ptr_ascii++]=temp%10+'0';
		buffer_ascii[ptr_ascii++]='/';

		temp=(date/2764800)%16;
		buffer_ascii[ptr_ascii++]=temp/10+'0';
		buffer_ascii[ptr_ascii++]=temp%10+'0';
		buffer_ascii[ptr_ascii++]='/';
	
		temp=date/44236800;
		buffer_ascii[ptr_ascii++]=temp/10+'0';
		buffer_ascii[ptr_ascii++]=temp%10+'0';
//	}
	
	buffer_ascii[ptr_ascii]=0;
	return(buffer_ascii);

}

char * time_ascii(xdata unsigned long val)
{
	val%=86400;
	buffer_ascii[ptr_ascii++]=val/36000+'0';
	buffer_ascii[ptr_ascii++]=(val/3600)%10+'0';
	buffer_ascii[ptr_ascii++]=':';
	buffer_ascii[ptr_ascii++]=(val/600)%6+'0';
	buffer_ascii[ptr_ascii++]=(val/60)%10+'0';
   buffer_ascii[ptr_ascii]=0;
	return(buffer_ascii);
}

char * txt_ascii(char *p_char)
{
	while(*p_char!=0)   
	{
	 	//wait_busy();
	 	buffer_ascii[ptr_ascii]=*p_char;
	 	p_char++;
	 	ptr_ascii++;
	}		
   buffer_ascii[ptr_ascii]=0;
	return(buffer_ascii);
}

char * data_ascii(xdata char val)
{
	buffer_ascii[ptr_ascii++]=val;
	buffer_ascii[ptr_ascii]=0;
	return(buffer_ascii);
}







/*
char * delay_ascii(unsigned long val)
{
   if(val>=3600)
   {
		if(val>=360000) buffer_ascii[ptr_ascii++]=(val/360000)%10+'0';
		if(val>=36000) buffer_ascii[ptr_ascii++]=(val/36000)%10+'0';
   	buffer_ascii[ptr_ascii++]=(val/3600)%10+'0';
		buffer_ascii[ptr_ascii++]=':';	
	}
	if(val>=600) buffer_ascii[ptr_ascii++]=(val/600)%6+'0';
	buffer_ascii[ptr_ascii++]=(val/60)%10+'0';	
	
	if(val<360000)
	{
		buffer_ascii[ptr_ascii++]=39;
		buffer_ascii[ptr_ascii++]=(val/10)%6+'0';
		buffer_ascii[ptr_ascii++]=val%10+'0';
	}
	buffer_ascii[ptr_ascii]=0;
	return(buffer_ascii);
}

*/
char * delay_ascii(xdata unsigned long val)   // formatage du temps en 5 caracteres  
{
	if(val>=360000) buffer_ascii[ptr_ascii++]=(val/360000)%10+'0';
	if(val>=36000) buffer_ascii[ptr_ascii++]=(val/36000)%10+'0';
  	if(val>=3600) 
  	{
  		buffer_ascii[ptr_ascii++]=(val/3600)%10+'0';
		buffer_ascii[ptr_ascii++]=':';	
  	}
  	if(val>=600) buffer_ascii[ptr_ascii++]=(val/600)%6+'0';
	if(val<360000) buffer_ascii[ptr_ascii++]=(val/60)%10+'0';	
   if(val<3600)
   {
     	buffer_ascii[ptr_ascii++]=39;
		buffer_ascii[ptr_ascii++]=(val/10)%6+'0';
		buffer_ascii[ptr_ascii++]=val%10+'0';
   }
   if(val<600)
   {
    	buffer_ascii[ptr_ascii++]=' ';
		buffer_ascii[ptr_ascii++]=' ';		
		buffer_ascii[ptr_ascii++]=' ';		
		buffer_ascii[ptr_ascii++]=' ';		   
   }

	buffer_ascii[ptr_ascii]=0;
	return(buffer_ascii);

}

char * ushort_ascii(xdata unsigned short val)
{/*
	if(val>=10000) buffer_ascii[ptr_ascii++]=val/10000+'0';
	if(val>=1000) buffer_ascii[ptr_ascii++]=(val/1000)%10+'0';
	if(val>=100) buffer_ascii[ptr_ascii++]=(val/100)%10+'0';
	if(val>=10) buffer_ascii[ptr_ascii++]=(val/10)%10+'0';
	buffer_ascii[ptr_ascii++]=val%10+'0';
	buffer_ascii[ptr_ascii]=0;
	return(buffer_ascii);*/
	ptr_ascii += sprintf(&buffer_ascii[ptr_ascii],"%d",val);
	buffer_ascii[ptr_ascii] = 0;
	return(buffer_ascii);
}


 







