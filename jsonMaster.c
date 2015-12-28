#include <stdio.h>
#include <proto/exec.h>
//#include <proto/intuition.h>
#include <libraries/gadtools.h>
//#include <proto/muimaster.h>
#include <libraries/mui.h>
//#include <proto/api_mui.h>
#include <proto/ttengine.h>
#include <libraries/ttengine.h>

#include "json.h"
#include "jsonutil.h"
#include "utfjson.h"
#include "ttbitmap.h"

#define reg(a) __asm(#a)
#define OBJ_WINDOW  123456      /* identyfikator przydatny do odszukania obiektu */
                                /* okna w funkcji MainLoop() i SetNotifications */
//https://github.com/filip-maryjanski/NVM
 								
struct json_node_state
{
	int curlevel;
	int curcount;
	json_value *curjson;
	int isfolded;
};
																
Object *Win, *App, *Listview, *String, *Info;
Object *Expand, *Fold;
Object *ExpandAll, *FoldAll;
Object *menu_about;
Object *about_btn;
Object *menu_load;
Object *jsonload_popup;
Object *menu_lastload;
Object *menu_exit;
Object *menu_json;
Object *menu_json_fold;
Object *menu_json_exall;
Object *menu_json_exlevel;
Object *InfoWin;
Object *menu_mui_prefs;
Object *menu_ttfutf_file;
Object *ttf_string=0;
Object *ttf_popup;
Object *ttbitmap_obj = 0;
APTR font = 0;
  struct Window *syswin = 0;
  struct RastPort *rp = 0;
BPTR Plik = NULL;
json_value *jo=0;
int expand_all=0;
char *no_string="no string\n";

struct Library *MUIMasterBase, *UtilityBase;
struct Library *IntuitionBase=0;
struct Library *TTEngineBase=0;
struct Library *GfxBase;
struct MUI_CustomClass *TTBitmapClass;

void jsontest()
{
	jo=loadJson("get_panoramas.js");
	if (jo)
	{
		char *nodename;
		json_value *nn=0;		                      
		nn=get_next_node(jo);		
            while (nn!=0)
            { 
				//cout << get_node_name(nn) << endl;
				nodename=get_node_name(nn);
                print_node_type(nn);
				//printf(" %s\n", get_node_name(nn));
				if (nn->type==json_integer)
				{	//printf(" %Ld \n", nn->u.integer);
					printf("%ld %ld\n", (nn->u.integer >> 32), (nn->u.integer & 0xFFFFFFFF));
					printf("%d %d\n", (nn->u.integer >> 32), (nn->u.integer & 0xFFFFFFFF));
				}
				nn=get_next_node(nn);
            }			
		json_value_free(jo);
		jo=0;
	}
}

void json_escape()
{
	int i;
	for(i=1;i<=8;i++)
	{
		*(json_type_tab[i]+3)='\033';
		*(json_type_tab[i]+4)='0'+i;
	}
}

void parse_insert(char *nazwa_pliku)
{
    json_value *nn=0;
    
    SetAttrs (Info, MUIA_Text_Contents, "parsing... ");	
  if (jo!=0)
	json_value_free(jo);
  
  jo=loadJson(nazwa_pliku);
  if (jo)
   {
    SetAttrs (Info, MUIA_Text_Contents, "parsed ok ");	      
    //SetAttrs (App, MUIA_Application_Sleep, TRUE, TAG_END);  /* kursor "zegarkowy" */
    //SetAttrs (*listview, MUIA_List_Quiet, TRUE, TAG_END);    /* blokuj� od�wie�anie listy */
	jsonlevel=0;
	expand_all=1;
	nn=get_next_node(jo);		
		while (nn!=0)
		{	
			DoMethod (Listview, MUIM_List_InsertSingle, nn, MUIV_List_Insert_Bottom);											
			nn=get_next_node(nn);
		}
    //SetAttrs (*listview, MUIA_List_Quiet, FALSE, TAG_END);   /* odblokowuj� od�wie�anie listy */
    //SetAttrs (App, MUIA_Application_Sleep, FALSE, TAG_END);  /* kursor normalny */
   }
  else
      SetAttrs (Info, MUIA_Text_Contents, "parser error... ");
} 

APTR *init_font(char *name)
{
    APTR font = 0;
    
    font = TT_OpenFont(
            //TT_FontFile, "PROGDIR:AndaleMo.ttf",
            //TT_FontFile, "PROGDIR:MCTIME.ttf",
            TT_FontFile, name,
           TT_FontSize, 14, 
            TAG_END);
    return font;
}

long FontLoad(Object *filestring reg(a2), Object **listview reg(a1))
{
    char *font_name = 0;
    GetAttr (MUIA_String_Contents, ttf_string , (long*)&font_name);
    printf("FOnt name: %s \n", font_name);
    if(font)
        TT_CloseFont(font);
    font = 0;
    
    font = init_font(font_name);
    if (font)
    {
        if (rp)
                TT_SetFont(rp, font);
        printf("OK. Font ready\n");
    }
    else
       printf("TTF error...\n");                              
}

/* Hook wywo�ywany po wybraniu pliku */

long CzytajPlik (Object *filestring reg(a2), Object **listview reg(a1))
 {
  char linia[1024];                  /* miejsce na wczytywane linie */
  char *nazwa_pliku;
  long pozycja, odczyt;
  	//char *nodename;
  //if (Plik) Close (Plik);            /* zamknij ewentualny poprzedni plik */
  DoMethod (*listview, MUIM_List_Clear);
  GetAttr (MUIA_String_Contents, filestring, (long*)&nazwa_pliku);
  
  parse_insert(nazwa_pliku);
  
  return 0;
 }
 
/* Hook konstrukcyjny */

//char *LiniaConstructor (long nn reg(a1), APTR mempool reg(a2))
struct json_node_state *LiniaConstructor (long nn reg(a1), APTR mempool reg(a2))
{
  struct json_node_state *element;
	//return (char *)nn;
  if (element = AllocPooled (mempool, sizeof(struct json_node_state)))
  {
    element->curjson = nn;
	element->curlevel = jsonlevel;
	element->curcount = json_get_size(nn);
	if (expand_all==1)
		element->isfolded = 0;	
	else
		element->isfolded=json_get_size(nn);
		
	return element;
  }
  return NULL;
 }

/* Hook destrukcyjny */

long LiniaDestructor (long *linia reg(a1), APTR mempool reg(a2))
 {
  if (linia) FreePooled (mempool, linia, sizeof (struct json_node_state));
 
  return 0;
 }

/* Hook wy�wietlaj�cy */

long LiniaDisplayer (struct json_node_state *ns reg(a1), char **teksty reg(a2))
 {
	static char *strobject=" {...}";
	static char *strarray=" [...]";
	static char *strtrue="true";
	static char *strfalse="false";
	static char *strtabs="                   |";
	static char *strtab2="                   +";
	static char text[100];
	static char name[100];	
	static char level[12];		
	char *nodename=0;
	json_value *nn;
	// !!!!!
	nn=ns->curjson;	
	// header
	if (!ns)
	{
		teksty[0] = "\33c\33b\0331Type";
		teksty[1] = "\33c\33b\0331Name";
		teksty[2] = "\33c\33b\0331Value";
		teksty[3] = "\33c\33b\0331Level";
		return 0;
	}
	// level
	sprintf(level, " %d:%d:%d", ns->curlevel, ns->curcount, ns->isfolded);
	teksty[3]=level;
	// name
	nodename=get_node_name(nn);
	if (nodename!=0)
	{
		if (ns->isfolded)
			sprintf(name, "%s%s", strtab2+20-ns->curlevel, nodename);	
		else
			sprintf(name, "%s%s", strtabs+20-ns->curlevel, nodename);			
		teksty[1] = name;
	}
	// type
	*teksty = json_type_tab[nn->type]+3;
	// value
	teksty[2]=0;
	if (nn->type==json_string)
		teksty[2]=nn->u.string.ptr;
	if (nn->type==json_array)
		teksty[2]=strarray;
	if (nn->type==json_object)
		teksty[2]=strobject;
	// konwersje na string
	if (nn->type==json_boolean)
		if (nn->u.boolean==1)
			teksty[2]=strtrue;
		else
			teksty[2]=strfalse;	
	if (nn->type==json_integer)
	{
		sprintf(text, "\"%lld\"", nn->u.integer);
		teksty[2]=text;
		//printf(" %ld \n", nn->u.integer);
	}
	if (nn->type==json_double)
	{
		sprintf(text, "\"%f\"", nn->u.dbl);
		teksty[2]=text;
	}
  return 0;
 }

long ExpandHook(Object *info reg(a2))
 {
	expand(info);
	return 0;
 }
 
long expand(Object *info)
{
  long poz=-1;
  long licznik=1;
  //long licznik2=0;  
  struct json_node_state *jnode=0;
  json_value *nn=0;
  //pobranie pozycji w lv oraz obiektu
  GetAttr (MUIA_List_Active, Listview, &poz);
  if (poz!=-1)
	DoMethod(Listview, MUIM_List_GetEntry, poz, &jnode); 
  else
	return 0;
	// czy mamy json node?
  if (jnode==0)
	return 0;
  if (jnode->isfolded==0)
	return 0;
	// !!!!!!!!!!!!!!
	expand_all=0;
	jnode->isfolded=0;
	jsonlevel=jnode->curlevel;		
	jnode->curcount=json_get_size(jnode->curjson);
  if (json_get_size(jnode->curjson)>0)
	nn=get_next_node(jnode->curjson);		
		while (nn!=0)
		{	
			DoMethod (Listview, MUIM_List_InsertSingle, nn, poz+(licznik++));											
			nn=get_flat_node(nn);
		}
  //DoMethod (info, MUIM_SetAsString, MUIA_Text_Contents, "Wpisano %ld ", liczba);	
  //printf(" Expand ! %ld :P\n", licznik);
  return 0;
 }
 
long FoldHook(Object *info reg(a2))
{
	fold(info);	
	return 0;
}

long fold(Object *info)
{
  long poz=-1;
  long licznik=0;
  long licznik2=0;  
  struct json_node_state *jnode=0;
  struct json_node_state *jnode_rest=0;
  GetAttr (MUIA_List_Active, Listview, &poz);
  if (poz!=-1)
	DoMethod(Listview, MUIM_List_GetEntry, poz, &jnode); 
  else
	return 0;
	// czy mamy json node?
  if (jnode==0)
	return 0;
  jnode_rest=jnode;
  DoMethod (info, MUIM_SetAsString, MUIA_Text_Contents, "Node data %ld:%ld:%ld", jnode->curlevel, jnode->curcount, jnode->isfolded);
  // folded... exit
  if (jnode->isfolded>0)
	return 0;
  // delete loop
  licznik=jnode->curcount;
    SetAttrs (Listview, MUIA_List_Quiet, TRUE, TAG_END); 
	while(licznik>0)
	{	
		jnode=0;
		DoMethod(Listview, MUIM_List_GetEntry, poz+1, &jnode);
		if (jnode==0)
			break;	
		licznik+=jnode->curcount;			
		if (jnode->isfolded>0)
			licznik2+=(int)(jnode->isfolded);
			//printf(" %ld \n", jnode->isfolded);
		licznik2+=1;
		//del current
		DoMethod(Listview,MUIM_List_Remove,poz+1);
		//licznik2+=1;
		licznik-=1;						
		//printf(" Liczniki %d %d %d\n", poz, licznik, licznik2);
	}
	SetAttrs (Listview, MUIA_List_Quiet, FALSE, TAG_END); 
	jnode_rest->isfolded=licznik2;
	jnode_rest->curcount=0;
	DoMethod(Listview,MUIM_List_Redraw, poz);
  return 0;
 }

int convert8utf16(char *utf8, char esc)
{
    short *txt16 = 0;
    int txt16len = 0;
    int i = 0;
    
    txt16len = utf_text_info(utf8, esc);
    txt16 = (short *)malloc((txt16len+1)*sizeof(short));
    txt16[txt16len] = 0x0000;   //important!!!
    if (txt16)
    {
        utf8to16(utf8, txt16, esc);        
        //for (i = 0; i < txt16len; i++)
          //  printf("%d. %x\n", i, txt16[i]);
        return txt16;
        //free(txt16);
    }
    else
        return 0;
}

long DoubleClickHook(Object *info reg(a2))
 {
  short *txt16 = 0;
  long poz=-1;  
  struct json_node_state *jnode=0;
  GetAttr (MUIA_List_Active, Listview, &poz);
  if (poz!=-1)
	DoMethod(Listview, MUIM_List_GetEntry, poz, &jnode); 
  else
	return 0;
	// czy mamy json node?
  if (jnode==0)
	return 0;
  // folded... exit
  if (jnode->curjson->type==json_string)
  {
        printf("String UTF8-16 ...\n");
        //utf_text_info(jnode->curjson->u.object.values,'z');
        //utf_text_info(jnode->curjson->u.string.ptr, 'u');
        //printf("%s\n", jnode->curjson->u.string.ptr);
        txt16 = convert8utf16(jnode->curjson->u.string.ptr, 'z');
        if (txt16) 
            //DoMethod(ttbitmap_obj, MUIM_Draw, MADF_DRAWOBJECT);
            MUI_Redraw(ttbitmap_obj, MADF_DRAWOBJECT);
        
        if (txt16)
        {
            printf("drawing...\n");
            TT_SetAttrs(rp, TT_Window, (ULONG)syswin, TAG_END);
            SetAPen(rp, 2);
            SetDrMd(rp, JAM1);
            Move(rp, 20, 40);
            TT_SetAttrs(rp,
                    TT_Antialias, TT_Antialias_Off, 
                    TT_Encoding, TT_Encoding_UTF16_BE, 
                    TAG_END);
            //TT_Text(rp, "This is a text printed with TT_Text().", 38);
            TT_Text(rp, txt16, 12);
            free(txt16);           
        }
        return 0;
  }
  if (jnode->isfolded>0)
	expand(info);
  else
	fold(info);
	//printf(" Klik! :P\n"); 
	return 0;	
 }
 
long close_about(Object* obj, struct MsgData* msg)
{
        SetAttrs (InfoWin, MUIA_Window_Open, FALSE, TAG_END);
}

struct Hook h_ExpandHook = {NULL, NULL, (HOOKFUNC)ExpandHook, NULL, NULL}; 
struct Hook h_FoldHook = {NULL, NULL, (HOOKFUNC)FoldHook, NULL, NULL}; 
struct Hook h_DoubleClickHook = {NULL, NULL, (HOOKFUNC)DoubleClickHook, NULL, NULL}; 
/* definicje struktur Hook */
struct Hook h_LiniaConstructor = {NULL, NULL, (HOOKFUNC)LiniaConstructor, NULL, NULL};
struct Hook h_LiniaDestructor = {NULL, NULL, (HOOKFUNC)LiniaDestructor, NULL, NULL};
struct Hook h_LiniaDisplayer = {NULL, NULL, (HOOKFUNC)LiniaDisplayer, NULL, NULL};
struct Hook h_CzytajPlik = {NULL, NULL, (HOOKFUNC)CzytajPlik, NULL, NULL};
struct Hook h_FontLoad = {NULL, NULL, (HOOKFUNC)FontLoad, NULL, NULL};
struct Hook h_close_about = {NULL, NULL, (HOOKFUNC)close_about, NULL, NULL};

// ==================================================

/* Fun GUI */

long BuildApplication (void)
{
    char copyright = 0xA9; 
    char *win_title = 0;
    char *screen_title = 0;
    
  App = MUI_NewObject (MUIC_Application,
   MUIA_Application_Author, (long)"Tomasz Okoniewski - virago/BlaBla)",
   MUIA_Application_Base, (long)"JSONMASTER",
   MUIA_Application_Copyright, screen_title=(long)"c 2014-2015 by BlaBla Corp.",
   MUIA_Application_Description, (long)"simple json files viewer",
   MUIA_Application_Title, (long)"jsonMaster",
   MUIA_Application_Version, (long)"$VER: jsonMaster 1.0 (21.12.2015) BLABLA PRODUCT",
   MUIA_Application_Menustrip, MUI_NewObject(MUIC_Menustrip,
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,                
			MUIA_Menu_Title, (long)"File",
			MUIA_Group_Child, (menu_load = MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"Open file",
				MUIA_Menuitem_Shortcut, (long)"O", 
			TAG_END)),							                        
			MUIA_Group_Child, MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, NM_BARLABEL,
			TAG_END),                        
			MUIA_Group_Child, (menu_about = MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"About",
				MUIA_Menuitem_Shortcut, (long)"?", 
			TAG_END)),							
			MUIA_Group_Child, MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, NM_BARLABEL,
			TAG_END),                        
			MUIA_Group_Child, (menu_exit = MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"Exit",
				MUIA_Menuitem_Shortcut, (long)"Q", 
			TAG_END)),                        
		TAG_END),
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
			MUIA_Menu_Title, (long)"Json",
			MUIA_Group_Child, (menu_json_exall = MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"Expand all",
				MUIA_Menuitem_Shortcut, (long)"A",                                 
			TAG_END)),
			MUIA_Group_Child, (menu_json_exlevel = MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"Expand level",
				MUIA_Menuitem_Shortcut, (long)"E",                                 
			TAG_END)),
			MUIA_Group_Child, (menu_json_fold = MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"Fold",
				MUIA_Menuitem_Shortcut, (long)"F",                                 
			TAG_END)),
		TAG_END),                        
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
			MUIA_Menu_Title, (long)"Prefs",
			MUIA_Group_Child, (menu_lastload = MUI_NewObject(MUIC_Menuitem,
                                //MUIA_ObjectID, (++i)+PREFSOBJECTID,
				MUIA_Menuitem_Title, (long)"Save last load folder",
				//MUIA_Menuitem_Shortcut, (long)"O", 
                                MUIA_Menuitem_Checkit, TRUE,
                                MUIA_Menuitem_Checked, FALSE,
                                MUIA_Menuitem_Toggle, TRUE,
                                MUIA_ObjectID, 0x01234569,                                  
			TAG_END)),                        
			MUIA_Group_Child, (menu_mui_prefs = MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"About MUI ",
			TAG_END)),
		TAG_END),
                
		MUIA_Group_Child, MUI_NewObject(MUIC_Menu,
			MUIA_Menu_Title, (long)"TTF font",
			MUIA_Group_Child, (menu_ttfutf_file = MUI_NewObject(MUIC_Menuitem,
				MUIA_Menuitem_Title, (long)"Select UTF-16 .ttf font",
                                MUIA_Menuitem_Shortcut, (long)"T",
			TAG_END)),                        
		TAG_END),
                
	TAG_END),
   MUIA_Application_Window, Win = MUI_NewObject (MUIC_Window,
    MUIA_Window_Title, win_title = (long)"jsonMaster c 2014-2015 by BlaBla Corp.",
    MUIA_Window_ID, 0x50525A4B,
    MUIA_UserData, OBJ_WINDOW,
    MUIA_Window_RootObject, MUI_NewObject (MUIC_Group,
      
      MUIA_Group_Child, MUI_NewObject (MUIC_Bitmap,
       MUIA_Background, MUII_TextBack,
       MUIA_Frame, MUIV_Frame_Text,
       //MUIA_Bitmap_Bitmap, img_bitmap,
       //MUIA_FixWidth, ppn->x,
       MUIA_FixHeight, 30,
       //MUIA_Image_FreeVert, TRUE,
       //MUIA_Image_FreeHoriz, TRUE,
       //MUIA_Bitmap_Width, ppn->x,
       MUIA_Bitmap_Height, 30,
       //MUIA_InnerTop, 0,
       //MUIA_InnerBottom, 0,
       //MUIA_InnerLeft, 0,
       //MUIA_InnerRight, 0,
	   //MUIA_InputMode, MUIV_InputMode_RelVerify,
	   MUIA_ShortHelp, (unsigned long)"strefa mazania",	   
     TAG_END),    
           // ttf
      MUIA_Group_Child, ttbitmap_obj = NewObject (TTBitmapClass->mcc_Class, NULL,
       MUIA_Frame, MUIV_Frame_Text,
       MUIA_Background, MUII_TextBack,
     TAG_END),       
     MUIA_Group_Child, MUI_NewObject (MUIC_Group,
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
     TAG_END),
     // ==============================================
          MUIA_Group_Child, MUI_NewObject (MUIC_Group,
      MUIA_Group_Horiz, TRUE,
      MUIA_Group_Child, MUI_NewObject (MUIC_Text,
       MUIA_Text_Contents, (long)"\33rFile",
       MUIA_Frame, MUIV_Frame_String,
       MUIA_FramePhantomHoriz, TRUE,
       MUIA_HorizWeight, 0,
      TAG_END),      
      MUIA_Group_Child, jsonload_popup = MUI_NewObject (MUIC_Popasl,
       MUIA_Popstring_String, String = MUI_NewObject (MUIC_String,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ObjectID, 0x01234568,         
        MUIA_CycleChain, TRUE,		
       TAG_END),
       MUIA_Popstring_Button, MUI_NewObject (MUIC_Image,
        MUIA_Image_Spec, MUII_PopFile,
        MUIA_ShortHelp, (long)" Select json file ;-) ",
        MUIA_Image_FontMatch, TRUE,
        MUIA_Frame, MUIV_Frame_ImageButton,
        MUIA_InputMode, MUIV_InputMode_RelVerify,
		MUIA_CycleChain, TRUE,
       TAG_END),
      TAG_END),
     TAG_END),
        // json file select ^^^^^^
	 MUIA_Group_Child, MUI_NewObject (MUIC_Group,       /* grupa z przyciskami */
      MUIA_Group_Horiz, TRUE,
      MUIA_Group_Child, Expand = MUI_NewObject (MUIC_Text,       /* przycisk lewy */
       MUIA_Frame, MUIV_Frame_Button,
       MUIA_Background, MUII_ButtonBack,
       MUIA_Font, MUIV_Font_Button,
       MUIA_Text_Contents, (long)"\33cExpand",
       MUIA_Text_HiChar, 'e',
       MUIA_ControlChar, 'e',
       MUIA_InputMode, MUIV_InputMode_RelVerify,
       MUIA_CycleChain, TRUE,
      TAG_END),
      MUIA_Group_Child, Fold = MUI_NewObject (MUIC_Text,       /* przycisk prawy */
       MUIA_Frame, MUIV_Frame_Button,
       MUIA_Background, MUII_ButtonBack,
       MUIA_Font, MUIV_Font_Button,
       MUIA_Text_Contents, (long)"\33cFold",
       MUIA_Text_HiChar, 'f',
       MUIA_ControlChar, 'f',
	   MUIA_InputMode, MUIV_InputMode_RelVerify,	   
       //MUIA_InputMode, MUIV_InputMode_Toggle,
       MUIA_CycleChain, TRUE,
      TAG_END),
      MUIA_Group_Child, ExpandAll = MUI_NewObject (MUIC_Text,       /* przycisk lewy */
       MUIA_Frame, MUIV_Frame_Button,
       MUIA_Background, MUII_ButtonBack,
       MUIA_Font, MUIV_Font_Button,
       MUIA_Text_Contents, (long)"\33cExpand All",
       MUIA_InputMode, MUIV_InputMode_RelVerify,
       MUIA_CycleChain, TRUE,
      TAG_END),
      MUIA_Group_Child, FoldAll = MUI_NewObject (MUIC_Text,       /* przycisk prawy */
       MUIA_Frame, MUIV_Frame_Button,
       MUIA_Background, MUII_ButtonBack,
       MUIA_Font, MUIV_Font_Button,
       MUIA_Text_Contents, (long)"\33cFold All",
	   MUIA_InputMode, MUIV_InputMode_RelVerify,	   
       MUIA_CycleChain, TRUE,
      TAG_END),
	  TAG_END),
     MUIA_Group_Child, Listview = MUI_NewObject (MUIC_Listview,
      MUIA_Listview_Input, TRUE,                        /* lista tylko do odczytu - bez kursora */
      MUIA_Listview_List, MUI_NewObject (MUIC_List,
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
     TAG_END),
	 MUIA_Group_Child, Info = MUI_NewObject (MUIC_Text,        /* pole informacyjne */
      MUIA_Frame, MUIV_Frame_Text,
      MUIA_Background, MUII_TextBack,
     TAG_END),
    TAG_END),
   TAG_END),
  TAG_END);

  win_title[11] = copyright;
  screen_title[0] = copyright;          
  
       InfoWin = 0;
    InfoWin = MUI_NewObject (MUIC_Window,
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
                        Child, Label1("\33c Version 0.1f" ),                        
                        Child, Label1("\33c copyrights 2014-2015 BlaBla "),
                        Child, Label1("\33c " ),                         
                        Child, Label1("\33c Tomasz 'virago' Okoniewski " ),
                        Child, Label1("\33c virago77@interia.pl" ),                        
                        Child, Label1("\33c " ),   
                        Child, Label1("\33c The program uses the library "),
                        Child, Label1("\33c by James McLaughlin "),                           
                        Child, Label1("\33c https://github.com/udp/json-parser"), 
                        Child, Label1("\33c " ),                           
                        Child, Label1("\33c thanks to the people"),
                        Child, Label1("\33c of irc channels #ppa & #appbeta"),
                        Child, Label1("\33c for your help and good advice"),                          
                        //Child, Label1("\33c " ),                         
                        //Child, Label1("\33c krashan, kaczus, stefkos " ),                        
                        //Child, Label1("\33c tygrys, widelec"),                                               
                MUIA_Group_Child, about_btn = MUI_NewObject (MUIC_Text,
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
    
    if (App)
        DoMethod(App, OM_ADDMEMBER, InfoWin);                
                
    if (InfoWin)                                                
        return (long)App;
    else
        if (App)
            DisposeObject(App);
                return 0;
  return (long)App;
}

/* Ustawienie notyfikacji na zamkni�cie okna */

void SetNotifications (void)
{
  DoMethod (Win, MUIM_Notify, MUIA_Window_CloseRequest,
        MUIV_EveryTime, App, 2, MUIM_Application_ReturnID,
        MUIV_Application_ReturnID_Quit);
  
   DoMethod(menu_exit, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
        /* about window */
   DoMethod(menu_about, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        InfoWin, 3, MUIM_Set, MUIA_Window_Open, TRUE);  
   
   DoMethod(menu_ttfutf_file, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        ttf_popup, 1, MUIM_Popstring_Open);     
   
   DoMethod(menu_load, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jsonload_popup, 1, MUIM_Popstring_Open);     
   
   DoMethod (InfoWin, MUIM_Notify, MUIA_Window_CloseRequest, MUIV_EveryTime,
        App, 2, MUIM_CallHook, &h_close_about );
   
   DoMethod (about_btn, MUIM_Notify, MUIA_Pressed, TRUE,   
        App, 2, MUIM_CallHook, &h_close_about );
   
   //DoMethod (menu_load, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
     //   App, 3, MUIM_CallHook, &h_CzytajPlik, Listview);
  /* Notyfikacja na wprowadzenie nazwy pliku */
  DoMethod (ttf_string, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self,
        2, MUIM_CallHook, &h_FontLoad);
  
  DoMethod (String, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self,
        3, MUIM_CallHook, &h_CzytajPlik, Listview);

  DoMethod (Expand, MUIM_Notify, MUIA_Pressed,
        FALSE, Info, 2, MUIM_CallHook, &h_ExpandHook);
  
  DoMethod(menu_json_exlevel, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        Info, 2, MUIM_CallHook, &h_ExpandHook);
     
  DoMethod (Fold, MUIM_Notify, MUIA_Pressed,
        FALSE, Info, 2, MUIM_CallHook, &h_FoldHook);
  
  DoMethod(menu_json_fold, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        Info, 2, MUIM_CallHook, &h_FoldHook);  
   
  DoMethod (Listview, MUIM_Notify, MUIA_Listview_DoubleClick,
        TRUE, Info, 2, MUIM_CallHook, &h_DoubleClickHook);  
   
  return;
}

/* p�tla g��wna programu */

void MainLoop (void)
 {
  long signals;

  DoMethod(App, MUIM_Application_Load, MUIV_Application_Load_ENV);  
  SetAttrs (Win, MUIA_Window_Open, TRUE, TAG_END);
  GetAttr(MUIA_Window_Window, Win, &syswin);
  rp = syswin->RPort;  
  printf("window: %x\n", syswin);
  printf("rastport: %x\n", rp);
  
  font = init_font("PROGDIR:AndaleMo.ttf");
  if (rp) TT_SetFont(rp, font);

  while (DoMethod (App, MUIM_Application_NewInput, &signals) !=
   MUIV_Application_ReturnID_Quit)
   {
    if (signals)
     {
      signals = Wait (signals | SIGBREAKF_CTRL_C);
      if (signals & SIGBREAKF_CTRL_C) break;
     }
   }
  DoMethod(App, MUIM_Application_Save, MUIV_Application_Save_ENV);
  SetAttrs (Win, MUIA_Window_Open, FALSE, TAG_END);
  return;
 }
 
/* g��wna funkcja programu */
//int main (void)
int main(int argc, char *argv[])
 {
  if (IntuitionBase = (struct IntuitionBase*)OpenLibrary ("intuition.library", 37))
   {
    if (UtilityBase = OpenLibrary ("utility.library", 37))
     {
      if (MUIMasterBase = OpenLibrary ("muimaster.library", 19))
       {
        if (TTEngineBase = OpenLibrary("ttengine.library", 6))
         {
          if (GfxBase = OpenLibrary("graphics.library", 39))              
           {
            if (TTBitmapClass = MUI_CreateCustomClass (NULL, MUIC_Area, NULL,
                sizeof (struct TTBitmap), TTBitmapDispatcher))
             {              
              if (BuildApplication ())
               {
		  //jsontest();
		json_escape();
		if (argc>1)
                        parse_insert(argv[1]);		  
                SetNotifications ();
                MainLoop ();
                MUI_DisposeObject (App);
          //if (Plik) Close (Plik);              /* zamknij ewentualnie otwarty plik */
                if (jo!=0)
                    json_value_free(jo);            
                if(font)
                    TT_CloseFont(font);
                TT_DoneRastPort(rp);              
               }
              MUI_DeleteCustomClass (TTBitmapClass);
             }
            CloseLibrary(GfxBase);           
           }
          CloseLibrary(TTEngineBase);
         }
        else
            printf("TTEngine library open error\n");
        CloseLibrary (MUIMasterBase);
       }
      CloseLibrary (UtilityBase);
     }
    CloseLibrary ((struct Library*)IntuitionBase);
   }
  return 0;
 }
 
																            
																				
																			
 
                                                                       
																	            
																				
																			
																			
                                                                       
																	            
																				
																																				
																								         
																				
																																										
																								