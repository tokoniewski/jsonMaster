/* 
 * File:   utfjson.h
 * Author: tokoniewski
 *
 * Created on 8 grudzień 2015, 15:43
 */

#ifndef UTFJSON_H
#define	UTFJSON_H

#ifdef	__cplusplus
extern "C" {
#endif

int utf_text_info(char *txt, char esc);
int utf8to16(char *utf8z16, short *txt16, char esc);
int json_utf16hack(char *buffer, int length, char esc);

#ifdef	__cplusplus
}
#endif

#endif	/* UTFJSON_H */

