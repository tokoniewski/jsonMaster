
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utfjson.h"

#define JSON_UTF_MAGIC 0x7F

int utf_text_info(char *txt, char esc)
{
    int bytelen=0;
    int charlen=0;
    int licz=0;
    char *txtptr=0;
    
    bytelen = strlen(txt);    
    printf("Text data len: %d\n", bytelen);
    
    txtptr=txt;
    while(*txtptr && (licz<bytelen))
    {
        //printf("%c:%hX ", *txtptr, *txtptr);
        if(*txtptr==JSON_UTF_MAGIC)
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
    printf("Text char len: %d\n", charlen);
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
        if(*txtptr==JSON_UTF_MAGIC) //0x5C)
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
            printf("|");                
            }               
        }
        else
        {
            *txt16 = 0x0000 + *txtptr;
            txtptr++;
            printf(".");
        }
        txt16++;
    }
    printf("\n");
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
    printf("%d chars in json. Esc sequences: %d\n", length, z);
}
