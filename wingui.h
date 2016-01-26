/* 
 * File:   wingui.h
 * Author: tokoniewski
 *
 * Created on 21 styczeń 2016, 13:15
 */


#ifndef WINGUI_H
#define	WINGUI_H

#include "jsonMaster.h"

#define MUI_MENU_BARLABEL      MUI_NewObject(MUIC_Menuitem, MUIA_Menuitem_Title, NM_BARLABEL, TAG_END) 

#ifdef	__cplusplus
extern "C" {
#endif
    
Object *create_button(char *label, char *control, LONG objid);
Object *create_menu(char *label, char *control, LONG objid);
Object *BuildInfoWin();
struct SearchBarObj *BuildSearchBar();
Object *BuildMenu();
Object *BuildTTFfontReq();
Object *BuildJsonFileReq();

#ifdef	__cplusplus
}
#endif

#endif	/* WINGUI_H */

