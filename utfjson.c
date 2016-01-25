
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <proto/muimaster.h>
#include <libraries/mui.h>
 
#include "utfjson.h"
#include "json.h"
#include "jsonutil.h"
#include "jsonMaster.h"

#define JSON_UTF_MAGIC 0x7F

char infochar[80];

int utf_text_info(char *txt, char esc)
{
    int bytelen=0;
    int charlen=0;
    int licz=0;
    char *txtptr=0;
    
    bytelen = strlen(txt);    
    
    txtptr=txt;
    while(*txtptr && (licz<bytelen))
    {
        //printf("%c:%hX ", *txtptr, *txtptr);
        if(*txtptr==0x5C)
        {           
            txtptr++;                                                  
            licz++;
                if(*txtptr==esc)
                {             
                    txtptr++;
                    //printf("%c%c%c%c", txtptr[0],txtptr[1],txtptr[2],txtptr[3]);                    
                    txtptr+=4;
                }                
            //printf("\n");         
        }
        else
        {
            txtptr++;
            licz++;
        }
    }            
    //printf("\n");
    charlen=licz;
    sprintf(infochar, "Text data: %d bytes, %d chars", bytelen, charlen);
    SetAttrs(findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, infochar); 
    return charlen;
}

int utf8to16(char *utf8z16, short *txt16, char esc)
{
    int bytelen=0;
    char *txtptr=0;
    
    bytelen = strlen(utf8z16);        
    txtptr = utf8z16;
    
    while(*txtptr)
    {
        if(*txtptr==0x5C)
        {
            txtptr++;
            if(*txtptr==esc)
            {
                txtptr++;
                *txt16 = 0x1000  * (txtptr[0] < 0x40 ? (txtptr[0] - 0x30) : (txtptr[0] - 0x57));                
                *txt16 += 0x0100 * (txtptr[1] < 0x40 ? (txtptr[1] - 0x30) : (txtptr[1] - 0x57));                
                *txt16 += 0x0010 * (txtptr[2] < 0x40 ? (txtptr[2] - 0x30) : (txtptr[2] - 0x57));                
                *txt16 += 0x0001 * (txtptr[3] < 0x40 ? (txtptr[3] - 0x30) : (txtptr[3] - 0x57));
                txtptr+=4;
            //printf("|");                
            }               
        }
        else
        {
            *txt16 = 0x0000 + *txtptr;
            txtptr++;
            //printf(".");
        }
        txt16++;
    }
    //printf("\n");
}

int utf16_esc_rev(char *buffer, int length, char esc)
{
    int i=0;
    while(i<length)
    {
        if ((*buffer)==JSON_UTF_MAGIC)
        {
            *buffer=0x5C;        // backslash
            buffer++;
            if ((*buffer)=='z')
                *buffer='u';             
        }
        buffer++;
        i++;
    }        
}

int json_utf16revert(json_value *js)
{
    if (js)
    {
        char *nodename;
        json_value *nn=0;
        
        nn=get_next_node(js);
        while (nn!=0)
        {    
            if (nn->type==json_string)
                utf16_esc_rev(nn->u.string.ptr, nn->u.string.length, 'u');
            nn=get_next_node(nn);
        }
    }
}

int json_utf16hack(char *buffer, int length, char esc)
{
    int i=0;
    int z=0;
    while(i<length)
    {
        if ((*buffer)==0x5C)
        {
                buffer++;
                if ((*buffer)=='u') 
                {
                    *buffer=esc;
                    buffer[-1]=JSON_UTF_MAGIC;
                    z++;
                }
                i++;
        }
        buffer++;
        i++;
    }
    sprintf(infochar, "%d chars in json. Esc sequences: %d", length, z);
    //return (int)infochar;
    //SetAttrs (Info, MUIA_Text_Contents, infochar);
    //printf("%d chars in json. Esc sequences: %d\n", length, z);
}








