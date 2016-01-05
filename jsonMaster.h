/* 
 * File:   jsonMaster.h
 * Author: tokoniewski
 *
 * Created on 5 styczeń 2016, 13:12
 */

#ifndef JSONMASTER_H
#define	JSONMASTER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <intuition/classes.h>

#define findobj(id, parent) (Object*)DoMethod(parent, MUIM_FindUData, id)

#define JM_OBJ_MENU_EXIT        0x500
#define JM_OBJ_MENU_OPENFILE    0x501
#define JM_OBJ_MENU_ABOUT       0x502

#define JM_OBJ_MENU_EXALL       0x510
#define JM_OBJ_MENU_EXLEV       0x511
#define JM_OBJ_MENU_FOLD        0x512
    
#define JM_OBJ_MENU_SELTTF      0x520
    
#define JM_OBJ_BUTTON_EXPAND    0x600
#define JM_OBJ_BUTTON_FOLD      0x601
#define JM_OBJ_BUTTON_EXALL     0x602
#define JM_OBJ_BUTTON_FOLDALL   0x603

#define   JM_App_Author         "Tomasz Okoniewski - virago/BlaBla"
#define   JM_App_Base           "JSONMASTER"
#define   JM_App_Copyright      "c 2014-2016 by BlaBla Corp."
#define   JM_App_Descr          "simple json files viewer"
#define   JM_App_Title          "jsonMaster"
#define   JM_App_Version        "$VER: jsonMaster 0.2 (5.01.2016) BLABLA PRODUCT"
    
extern Object *App, *Win;

#ifdef	__cplusplus
}
#endif

#endif	/* JSONMASTER_H */

