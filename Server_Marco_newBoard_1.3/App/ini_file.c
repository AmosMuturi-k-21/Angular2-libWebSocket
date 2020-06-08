/*
 * ini_file.c
 *
 *  Created on: May 24, 2019
 *      Author: Marco Sartore
 */



// general purpose includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "vardef.h"

#define	DEFAULT_VALDET_OFFSET	0.0
#define	DEFAULT_VALMIR_OFFSET	0.0
#define	DEFAULT_VALSURF_OFFSET	0.0


#define MAX_SECTIONS	10
#define MAX_FIELDS		5

#ifndef	TRUE
 #define TRUE			1
 #define FALSE			0
#endif


// types definition
struct param_ID {
    int sec_idx;
    int field_idx;
    char *label;
};
struct cfgField {
    char *name;
    char *strValue;
    double dValue;
};
struct cfgSection {
    char *name;
    struct cfgField Fields[MAX_FIELDS];
    int num_fields;
};

// global variables
struct cfgSection Sections[MAX_SECTIONS];
int num_sections;
char tokglob[255];
FILE *fp;

// prototypes
int  myStrlen(char *);
void mystrsep(char[], int);
unsigned short us2bytes(float);
void TxByte(char);
void FieldVal_str2double(int, int);
void FieldVal_long2str(int, int);
void Field_GetName(int, int, char *);
double FieldVal_GetNum(int, int);
void FieldVal_GetStr(int, int, char *);
void Field_SetName(int, int, char *);
void FieldVal_SetNum(int, int, double);
void FieldVal_SetStr(int, int, char *);
void apply_config_defaults(void);
void Update_ParamID(void);

void ReadConfFile(void);
void WriteConfFile(void);



// *******************************************************************************
// myStrlen
// *******************************************************************************
int myStrlen( char *s )
{
    char *start;
    start = s;
    while( *s != 0 )
    {
        ++s;
    }
    return s - start;
}



// *******************************************************************************
// mystrsep
// *******************************************************************************
void mystrsep(char *strpar, int toknum)  // toknum starts from 1, where 1=first token
{
	int i, l, j;
	int colon = 0;
	char *s;

	s = strpar;
	l = myStrlen(s);
	j = 0;
	for (i = 0; (i < l) && (colon < toknum); i++)
	{
		if(s[i] != ':')
		{
			tokglob[j++] = s[i];
		}
		else
		{
			tokglob[j] = '\0';
			j = 0;
			colon++;
		}
	}
}



// *******************************************************************************
// FieldVal_str2double
// *******************************************************************************
void FieldVal_str2double(int i, int j)
{
    char *tmpPtr;

    Sections[i].Fields[j].dValue = strtod(Sections[i].Fields[j].strValue, &tmpPtr);
}



// *******************************************************************************
// FieldVal_long2str
// *******************************************************************************
void FieldVal_long2str(int i, int j)
{
    char tmpStr[20];

    sprintf(tmpStr, "%lf", Sections[i].Fields[j].dValue);
    free(Sections[i].Fields[j].strValue);
    Sections[i].Fields[j].strValue = (char*) malloc((myStrlen(tmpStr)+1)*sizeof(char));
    strcpy(Sections[i].Fields[j].strValue, tmpStr);
}



// *******************************************************************************
// Field_GetName
// *******************************************************************************
void Field_GetName(int i, int j, char *name)
{
    strcpy(name, Sections[i].Fields[j].name);
}



// *******************************************************************************
// FieldVal_GetNum
// *******************************************************************************
double FieldVal_GetNum(int i, int j)
{
	double d;
	
    d = Sections[i].Fields[j].dValue;
	
	return (d);
}



// *******************************************************************************
// FieldVal_GetStr
// *******************************************************************************
void FieldVal_GetStr(int i, int j, char *val)
{
    strcpy(val, Sections[i].Fields[j].strValue);
}



// *******************************************************************************
// Field_SetName
// *******************************************************************************
void Field_SetName(int i, int j, char *name)
{
    free(Sections[i].Fields[j].name);
    Sections[i].Fields[j].name = (char*) malloc((myStrlen(name)+1)*sizeof(char));
    strcpy(Sections[i].Fields[j].name, name);
}


// *******************************************************************************
// FieldVal_SetNum
// *******************************************************************************
void FieldVal_SetNum(int i, int j, double val)
{
    Sections[i].Fields[j].dValue = val;
}



// *******************************************************************************
// FieldVal_SetStr
// *******************************************************************************
void FieldVal_SetStr(int i, int j, char *val)
{
    free(Sections[i].Fields[j].strValue);
    Sections[i].Fields[j].strValue = (char*) malloc((myStrlen(val)+1)*sizeof(char));
    strcpy(Sections[i].Fields[j].strValue, val);

}



// *******************************************************************************
// InitVars
// *******************************************************************************
void InitVars(void)
{
    num_sections = 1;
    Sections[0].num_fields = 0;
}



// *******************************************************************************
// apply_config_defaults
//
// *******************************************************************************
void apply_config_defaults(void)
{
	valdet_offset  = DEFAULT_VALDET_OFFSET;
	valmir_offset  = DEFAULT_VALMIR_OFFSET;
	valsurf_offset = DEFAULT_VALSURF_OFFSET;
}



// *******************************************************************************
// ReadConfFile
// *******************************************************************************
void ReadConfFile(void)
{
	char line[255];
    char firstChar, c;
    char tmpSecName[30], tmpFieldName[30], tmpFieldVal[30];

	fp = fopen("hwconfig.ini","r");
	if(fp == NULL)
	{
		printf("\nUnable to open the configuration file, applying defaults...\n\n");        
		apply_config_defaults();
	}
	else
	{
		while (fgets(line, 255, fp) != NULL)
		{
			//printf("\nline: %s", line);
            firstChar = ' ';
            sscanf(line, "%c", &firstChar);
            if(firstChar > 0x20)
            {
                if(firstChar == '[')
                {
                    num_sections++;
                    sscanf(line, "%s", tmpSecName);
                    Sections[num_sections - 1].name = (char*) malloc((myStrlen(tmpSecName)+1)*sizeof(char));
                    strcpy(Sections[num_sections - 1].name, tmpSecName);
                    Sections[num_sections - 1].num_fields = 0;
					//printf("\nsection %d", Sections[num_sections - 1].num_fields);
                }
                else if(num_sections >= 1)
                {
                    Sections[num_sections - 1].num_fields++;
                    sscanf(line, "%s %c %s", tmpFieldName, &c, tmpFieldVal);
                    Field_SetName(num_sections - 1, Sections[num_sections - 1].num_fields - 1, tmpFieldName);
                    FieldVal_SetStr(num_sections - 1, Sections[num_sections - 1].num_fields - 1, tmpFieldVal);
                    FieldVal_str2double(num_sections - 1, Sections[num_sections - 1].num_fields - 1);
					//printf("\n%d %s %s ", Sections[num_sections - 1].num_fields, tmpFieldName, tmpFieldVal);
                }

            }

		}
        fclose(fp);
    }

}



// *******************************************************************************
// Update_ParamID
// *******************************************************************************
void Update_ParamID(void)
{
    int i, j;
    for(i = 0; i < num_sections; i++)
    {
        for(j = 0; j < Sections[i].num_fields; j++)
        {
            if( strcmp(Sections[i].Fields[j].name, "valdet_offset") == 0)
		    {
            	valdet_offset = Sections[i].Fields[j].dValue;
				
		    }
            if( strcmp(Sections[i].Fields[j].name, "valmir_offset") == 0)
		    {
            	valmir_offset = Sections[i].Fields[j].dValue;
				
		    }
			if( strcmp(Sections[i].Fields[j].name, "valsurf_offset") == 0)
		    {
            	valsurf_offset = Sections[i].Fields[j].dValue;
				
		    }
        }
    }
}


// *******************************************************************************
// WriteConfFile
// *******************************************************************************
void WriteConfFile(void)
{
    int i, j;

    fp = fopen("tmpConfig.ini","w");
	if(fp == NULL)
	{
		printf("\nUnable to open the configuration file, keeping old configuration...\n\n");
	}
    else
    {
        for(i = 1; i < num_sections; i++)
        {
            fprintf(fp, "%s\n", Sections[i].name);
            for(j = 0; j < Sections[i].num_fields; j++)
            {
                fprintf(fp, "%s = %s\n", Sections[i].Fields[j].name, Sections[i].Fields[j].strValue);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
        rename("tmpConfig.ini", "hwconfig.ini");
    }

}



// *******************************************************************************
// read_hw_ini_file()
// *******************************************************************************
int read_hw_ini_file(void)
{
	
	// read the configuration file and assign the respective variables
	printf("\nReading configuration INI file...\n");

	apply_config_defaults();
	ReadConfFile();
    Update_ParamID();
	printf("valdet_offset  set to %lf\n", valdet_offset);
	printf("valmir_offset  set to %lf\n", valmir_offset);
	printf("valsurf_offset set to %lf\n", valsurf_offset);
	printf("\n");

	return 0;
}




