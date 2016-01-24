/* 
 * File:   jsonMaster.h
 * Author: tokoniewski
 *
 * Created on 5 styczeń 2016, 13:12
 */

#ifndef JSONMASTER_H
#define	JSONMASTER_H

#define OBJ_WINDOW  123456      /* identyfikator przydatny do odszukania obiektu */

#ifdef	__cplusplus
extern "C" {
#endif

#include <intuition/classes.h>

#define findobj(id, parent) (Object*)DoMethod(parent, MUIM_FindUData, id)

#define JM_OBJ_MENU_EXIT        0x500
#define JM_OBJ_MENU_OPENFILE    0x501
#define JM_OBJ_MENU_ABOUT       0x502
#define JM_OBJ_MENU_SAVEXML     0x503
    
#define JM_OBJ_MENU_EXALL       0x510
#define JM_OBJ_MENU_EXLEV       0x511
#define JM_OBJ_MENU_FOLD        0x512
    
#define JM_OBJ_MENU_SELTTF      0x520
#define JM_OBJ_MENU_SIZE        0x521

#define JM_OBJ_LVIEW            0x530
#define JM_OBJ_LVIEW_LIST       0x531
    
#define JM_OBJ_BUTTON_EXPAND    0x600
#define JM_OBJ_BUTTON_FOLD      0x601
#define JM_OBJ_BUTTON_EXALL     0x602
#define JM_OBJ_BUTTON_FOLDALL   0x603

#define JM_OBJ_BUTTON_LINEINFO  0x609
#define JM_OBJ_BUTTON_INFO      0x610  
#define JM_OBJ_BTN_POPUP_JSON   0x611

#define JM_OBJ_BTN_SEARCH_NEXT  0x620
#define JM_OBJ_BTN_SEARCH_PREV  0x621
#define JM_OBJ_STR_SEARCH       0x622
    
#define JM_OBJ_BTN_ABOUT_OK     0x630    

#define JM_DEFAULT_FONT_SIZE    14   
#define JM_DEFAULT_FONT_NAME    "PROGDIR:DejaVuSans.ttf"

#define   JM_App_Author         "Tomasz Okoniewski - virago/BlaBla"
#define   JM_App_Base           "JSONMASTER"
#define   JM_App_Copyright      "c 2014-2016 by BlaBla group"
#define   JM_App_Descr          "simple json files viewer"
#define   JM_App_Title          "jsonMaster"
#define   JM_App_Version        "$VER: jsonMaster 0.2 (21.01.2016) BLABLA PRODUCT"
    
extern Object *App, *Win;

#ifdef	__cplusplus
}
#endif

#endif	/* JSONMASTER_H */

