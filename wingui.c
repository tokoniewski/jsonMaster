
#include <libraries/mui.h>
//#include "proto/muimaster.h"
#include <libraries/gadtools.h>
#include <libraries/asl.h>


#include "jsonMaster.h"
#include "wingui.h"

extern Object *ttf_popup;
extern Object *ttf_string;
extern Object *String;

extern struct Hook h_LiniaConstructor;
extern struct Hook h_LiniaDestructor;
extern struct Hook h_LiniaDisplayer;

BarObj sbarobj;

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
                        Child, Label1("\33c " ),              
                        Child, Label1("\33c Version 0.2" ),                        
                        Child, Label1("\33c copyrights 2014-2016 BlaBla group"),
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

BarObj *BuildSearchBar()
{
    Object ob = 0;
    Object ob2 = 0;
    
        ob2 = MUI_NewObject (MUIC_String,
                        MUIA_String_Format, MUIV_String_Format_Right,
                        //MUIA_String_Integer, TRUE, 
                        MUIA_Frame, MUIV_Frame_Text,
                        MUIA_Background, MUII_TextBack,
                        MUIA_UserData, JM_OBJ_STR_SEARCH_POS,
                        MUIA_HorizWeight, 10,
                TAG_END);
        
        ob = MUI_NewObject (MUIC_Group,
                MUIA_Group_Horiz, TRUE,
                MUIA_FrameTitle, (long)"Search",                
                MUIA_Frame, MUIV_Frame_Group,
                MUIA_Background, MUII_GroupBack,
                TAG_END);
                
                MUIA_Group_Child, MUI_NewObject (MUIC_String,
                        //MUIA_ShowMe, FALSE,          
                        MUIA_Frame, MUIV_Frame_String,
                        //MUIA_ObjectID, 0x01234567,                                                
                        MUIA_CycleChain, TRUE,	
                        MUIA_UserData, JM_OBJ_STR_SEARCH,
                        MUIA_HorizWeight, 70,
                        TAG_DONE),
                //MUIA_Group_Child, ob2,
                MUIA_Group_Child, create_button("\33cPrev", 0, JM_OBJ_BTN_SEARCH_PREV),                        
                MUIA_Group_Child, create_button("\33cNext", 0, JM_OBJ_BTN_SEARCH_NEXT),
                //TAG_END);
        SetAttrs(findobj(JM_OBJ_BTN_SEARCH_NEXT, ob), MUIA_HorizWeight, 15, TAG_END);
        SetAttrs(findobj(JM_OBJ_BTN_SEARCH_PREV, ob), MUIA_HorizWeight, 15, TAG_END);        
        //SetAttrs(findobj(JM_OBJ_BTN_SEARCH_NEXT, ob), MUIA_Disabled, TRUE, TAG_END);
        //SetAttrs(findobj(JM_OBJ_BTN_SEARCH_PREV, ob), MUIA_Disabled, TRUE, TAG_END);  

        //if (ob)
          //  if (ob2)
            //    DoMethod(findobj(JM_OBJ_STR_SEARCH, ob), MUIM_Family_Insert, ob2, findobj(JM_OBJ_BTN_SEARCH_PREV, ob));
                //DoMethod(ob, MUIM_Family_AddHead, ob2);
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
			//MUIA_Group_Child, MUI_NewObject(MUIC_Menuitem,
			//	MUIA_Menuitem_Title, (long)"About MUI ",
			//TAG_END),
		TAG_END),                
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
			MUIA_Menu_Title, (long)"TTF font",
			MUIA_Group_Child, create_menu("Select UTF-16 .ttf font", "T", JM_OBJ_MENU_SELTTF),
                        MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
				MUIA_Menu_Title, (long)"Size",      
                                MUIA_Menuitem_Shortcut, (char *)"Z",
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
                        MUIA_ObjectID, 0x01230000 + (i*2) + 10,
                        MUIA_Menuitem_Checkit, TRUE,
                        MUIA_Menuitem_Checked, FALSE,        
                        MUIA_Menuitem_Exclude, 0x01F & ~mask,
                    TAG_END));  
        }
        SetAttrs(findobj(JM_DEFAULT_FONT_SIZE, m), MUIA_Menuitem_Checked, TRUE);
    return m;
}

Object *BuildTTFfontReq()
{
    Object *o = 0;
    o = MUI_NewObject (MUIC_Group,
      MUIA_Group_Horiz, TRUE,
      MUIA_Group_Child, MUI_NewObject (MUIC_Text,
       MUIA_Text_Contents, (long)"\33rTTF Font",
       MUIA_Frame, MUIV_Frame_String,
       MUIA_FramePhantomHoriz, TRUE,
       MUIA_HorizWeight, 0,
      TAG_END),
      MUIA_Group_Child, ttf_popup = MUI_NewObject (MUIC_Popasl,
        //MUIA_ShowMe, TRUE,      
        MUIA_Popstring_String, ttf_string = MUI_NewObject (MUIC_String,
                //MUIA_ShowMe, FALSE,          
                MUIA_Frame, MUIV_Frame_String,
                MUIA_ObjectID, 0x01234567,                                                
                MUIA_CycleChain, TRUE,		
        TAG_END),
        MUIA_Popstring_Button, MUI_NewObject (MUIC_Image,
                MUIA_Image_Spec, MUII_PopFile,
                MUIA_ShortHelp, (long)" Select ttf font ",
                MUIA_Image_FontMatch, TRUE,
                MUIA_Frame, MUIV_Frame_ImageButton,
                MUIA_InputMode, MUIV_InputMode_RelVerify,
		MUIA_CycleChain, TRUE,
       TAG_END),
      TAG_END),      
     TAG_END);
    return o;
}

Object *BuildJsonFileReq()
{
    Object *o = 0;
    o = MUI_NewObject(MUIC_Group,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Child, MUI_NewObject(MUIC_Text,
                MUIA_Text_Contents, (long)"\33rFile",
                MUIA_Frame, MUIV_Frame_String,
                MUIA_FramePhantomHoriz, TRUE,
                MUIA_HorizWeight, 0,
        TAG_END),      
        MUIA_Group_Child, /*jsonload_popup =*/ MUI_NewObject(MUIC_Popasl,
                MUIA_UserData, JM_OBJ_BTN_POPUP_JSON,
                MUIA_Popasl_Type, ASL_FileRequest,          
                MUIA_Popstring_String, String = MUI_NewObject(MUIC_String,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_ObjectID, 0x01234568,         
                        MUIA_CycleChain, TRUE,		
                TAG_END),
                MUIA_Popstring_Button, MUI_NewObject(MUIC_Image,
                        MUIA_Image_Spec, MUII_PopFile,
                        MUIA_ShortHelp, (long)" Select json file ;-) ",
                        MUIA_Image_FontMatch, TRUE,
                        MUIA_Frame, MUIV_Frame_ImageButton,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_CycleChain, TRUE,
                TAG_END),
        TAG_END),
    TAG_END);
    return o;
}

Object *BuildListview()
{
    Object lv = 0;
    lv = MUI_NewObject(MUIC_Listview,
        MUIA_Listview_Input, TRUE,                        /* lista tylko do odczytu - bez kursora */
        MUIA_UserData, JM_OBJ_LVIEW,                        
        MUIA_Listview_List, MUI_NewObject (MUIC_List,
                MUIA_UserData, JM_OBJ_LVIEW_LIST,
                MUIA_List_ConstructHook, (long)&h_LiniaConstructor,
                MUIA_List_DestructHook, (long)&h_LiniaDestructor,
                MUIA_List_DisplayHook, (long)&h_LiniaDisplayer,
                //MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
                //MUIA_List_DestructHook, MUIV_List_DestructHook_String,
                MUIA_List_Format, (long)"BAR PREPARSE=\33r,BAR MAXWIDTH=-1,,PREPARSE=\33r",
                MUIA_List_Title, TRUE,
                MUIA_Frame, MUIV_Frame_ReadList,
                MUIA_Font, MUIV_Font_Fixed,
                TAG_END),
        MUIA_Listview_DoubleClick, TRUE,
        MUIA_CycleChain, TRUE,
        TAG_END);    
    return lv;
}




