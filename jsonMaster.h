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
        
extern Object *App, *Win;

#ifdef	__cplusplus
}
#endif

#endif	/* JSONMASTER_H */

