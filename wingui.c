
#include <libraries/mui.h>
//#include "proto/muimaster.h"
#include <libraries/gadtools.h>

#include "jsonMaster.h"
#include "wingui.h"


Object *create_menu(char *label, char *control, LONG objid)
{
        Object *obj;
        obj = MUI_NewObject(MUIC_Menuitem,
                MUIA_Menuitem_Title, (ULONG)label,
                MUIA_Menuitem_Shortcut, control,
                MUIA_UserData, objid,                 
                TAG_END);
        return obj;
}

Object *create_button(char *label, char *control, LONG objid)
{
        Object *obj;
        obj = MUI_NewObject (MUIC_Text,       /* przycisk */
                MUIA_Frame, MUIV_Frame_Button,
                MUIA_Background, MUII_ButtonBack,
                MUIA_Font, MUIV_Font_Button,
                MUIA_Text_Contents, (ULONG)label,
                MUIA_Text_HiChar, control,
                MUIA_ControlChar, control,
                MUIA_InputMode, MUIV_InputMode_RelVerify,
                MUIA_CycleChain, TRUE,
                MUIA_UserData, objid,
        TAG_END);
        return obj;
}      

Object *BuildInfoWin()
{
    Object *o = 0;
    o = MUI_NewObject (MUIC_Window,
        MUIA_Window_Title, (long)"jsonMaster Information",
        MUIA_Window_ID, 0x50525A4C,
        MUIA_UserData, OBJ_WINDOW+1,
        MUIA_Window_RootObject, MUI_NewObject (MUIC_Group,
                MUIA_Group_Child, MUI_NewObject (MUIC_Group,
                        MUIA_FrameTitle, (long)"About",
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_Background, MUII_GroupBack,                
                        MUIA_Group_Horiz, FALSE,        
                        MUIA_Group_Child, MUI_NewObject (MUIC_Text,
                                MUIA_Text_Contents, (long)"\33c\33b json Master", 
                        TAG_END),                                                
                        Child, Label1("\33c Version 0.2" ),                        
                        Child, Label1("\33c copyrights 2014-2016 BlaBla "),
                        Child, Label1("\33c " ),                         
                        Child, Label1("\33c Tomasz 'virago' Okoniewski " ),
                        Child, Label1("\33c virago77@interia.pl" ),                        
                        Child, Label1("\33c " ),   
                        Child, Label1("\33c The program uses the library "),
                        Child, Label1("\33c by James McLaughlin "),                           
                        Child, Label1("\33c https://github.com/udp/json-parser"), 
                        Child, Label1("\33c " ),                           
                        Child, Label1("\33c thanks to the people"),
                        Child, Label1("\33c of irc channel #ppa "),
                        Child, Label1("\33c for your help and good advice"),                          
                        //Child, Label1("\33c " ),                         
                        //Child, Label1("\33c krashan, kaczus, stefkos " ),                        
                        //Child, Label1("\33c tygrys, widelec"),                                               
                        MUIA_Group_Child, /*about_btn =*/ MUI_NewObject (MUIC_Text,
                                MUIA_UserData, JM_OBJ_BTN_ABOUT_OK,
                                MUIA_Frame, MUIV_Frame_Button,
                                MUIA_Background, MUII_ButtonBack,
                                MUIA_Font, MUIV_Font_Button,                        
                                MUIA_Text_Contents, (long)"\33cOK",
                                MUIA_InputMode, MUIV_InputMode_RelVerify,
                                MUIA_CycleChain, TRUE,   
                        TAG_END),                        
                TAG_END),                  
          TAG_END),
    TAG_END);    
    return o;
}

Object *BuildSearchBar()
{
    Object ob = 0;
        ob = MUI_NewObject (MUIC_Group,
                MUIA_Group_Horiz, TRUE,
                MUIA_FrameTitle, (long)"Search",                
                MUIA_Frame, MUIV_Frame_Group,
                MUIA_Background, MUII_GroupBack,
                MUIA_Group_Child, MUI_NewObject (MUIC_String,
                        //MUIA_ShowMe, FALSE,          
                        MUIA_Frame, MUIV_Frame_String,
                        //MUIA_ObjectID, 0x01234567,                                                
                        MUIA_CycleChain, TRUE,	
                        MUIA_UserData, JM_OBJ_STR_SEARCH,
                        MUIA_HorizWeight, 70,
                        TAG_DONE),
                MUIA_Group_Child, create_button("\33cPrev", 0, JM_OBJ_BTN_SEARCH_PREV),                        
                MUIA_Group_Child, create_button("\33cNext", 0, JM_OBJ_BTN_SEARCH_NEXT),
                TAG_END);
        SetAttrs(findobj(JM_OBJ_BTN_SEARCH_NEXT, ob), MUIA_HorizWeight, 15, TAG_END);
        SetAttrs(findobj(JM_OBJ_BTN_SEARCH_PREV, ob), MUIA_HorizWeight, 15, TAG_END);        
        //SetAttrs(findobj(JM_OBJ_BTN_SEARCH_NEXT, ob), MUIA_Disabled, TRUE, TAG_END);
        //SetAttrs(findobj(JM_OBJ_BTN_SEARCH_PREV, ob), MUIA_Disabled, TRUE, TAG_END);                
        return ob;
}

Object *BuildMenu()
{
    Object *m = 0;
    int i, mask;
    char *size[5] = {"10","12","14","16","18"};
    //int *size2 = *size;
    m = MUI_NewObject(MUIC_Menustrip,
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,                
			MUIA_Menu_Title, (long)"File",
			MUIA_Group_Child, create_menu("Open file", "O", JM_OBJ_MENU_OPENFILE),	
			MUIA_Group_Child, create_menu("Save as XML", "X", JM_OBJ_MENU_SAVEXML),	
			MUIA_Group_Child, MUI_MENU_BARLABEL,
			MUIA_Group_Child, create_menu("About", "?", JM_OBJ_MENU_ABOUT),
			MUIA_Group_Child, MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, NM_BARLABEL,
			TAG_END),                        
			MUIA_Group_Child, create_menu("Exit", "Q", JM_OBJ_MENU_EXIT),
		TAG_END),
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
			MUIA_Menu_Title, (long)"Json",
			MUIA_Group_Child, create_menu("Expand all","A", JM_OBJ_MENU_EXALL), 
			MUIA_Group_Child, create_menu("Expand level", "E", JM_OBJ_MENU_EXLEV),
			MUIA_Group_Child, create_menu("Fold", "F", JM_OBJ_MENU_FOLD),
		TAG_END),                        
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
			MUIA_Menu_Title, (long)"Prefs",
			MUIA_Group_Child, (/*menu_lastload =*/ MUI_NewObject(MUIC_Menuitem,
                                //MUIA_ObjectID, (++i)+PREFSOBJECTID,
				MUIA_Menuitem_Title, (long)"Save last load folder",
				//MUIA_Menuitem_Shortcut, (long)"O", 
                                MUIA_Menuitem_Checkit, TRUE,
                                MUIA_Menuitem_Checked, FALSE,
                                MUIA_Menuitem_Toggle, TRUE,
                                MUIA_ObjectID, 0x01234569,                                  
			TAG_END)),                        
			MUIA_Group_Child, MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"About MUI ",
			TAG_END),
		TAG_END),                
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
			MUIA_Menu_Title, (long)"TTF font",
			MUIA_Group_Child, create_menu("Select UTF-16 .ttf font", "T", JM_OBJ_MENU_SELTTF),
                        MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
				MUIA_Menu_Title, (long)"Size",      
                                MUIA_Menuitem_Shortcut, "Z",
                                //MUIA_ShortHelp, "Select font size",
                                MUIA_UserData, JM_OBJ_MENU_SIZE,
                        TAG_END),
                        //======================
		TAG_END),                
	TAG_END);
        for (i=0; i<5; i++)
	{                   
            mask=(1<<(i+0));
            //size[2] = (int)'0' + i*2;
            DoMethod(findobj(JM_OBJ_MENU_SIZE, m), MUIM_Family_AddTail,
                    MUI_NewObject(MUIC_Menuitem,
                        MUIA_Menuitem_Title, size[i],
                        //MUIA_Menuitem_Shortcut, '0'+ (i*2),
                        MUIA_UserData, (ULONG)((i*2) + 10),
                        MUIA_Menuitem_Checkit, TRUE,
                        MUIA_Menuitem_Checked, FALSE,        
                        MUIA_Menuitem_Exclude, 0x01F & ~mask,
                    TAG_END));  
        }
    return m;
}



