#include <stdio.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <workbench/workbench.h>
//#include <proto/intuition.h>
#include <libraries/gadtools.h>
#include <proto/muimaster.h>
#include <libraries/mui.h> 
//#include <proto/api_mui.h>
#include <proto/ttengine.h>
#include <libraries/ttengine.h>
#include <libraries/asl.h>
   
#include "jsonMaster.h"
#include "json.h"
#include "jsonutil.h"
#include "utfjson.h"
#include "ttbitmap.h"
#include "wingui.h"

#define reg(a) __asm(#a)
                                /* okna w funkcji MainLoop() i SetNotifications */
//https://github.com/filip-maryjanski/NVM
 								
struct json_node_state
{
	int curlevel;
	int curcount;
	json_value *curjson;
	int isfolded;
};
																
Object *Win, *App, *Listview, *String;
//Object *menu_lastload;
Object *InfoWin;
Object *ttf_string = 0;
Object *ttf_popup;
Object *ttbitmap_obj = 0;
APTR font = 0;
//BPTR Plik = NULL;
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

void json2xml()
{
    if (jo)
    {
        char *nodename;
        json_value *nn=0;
        rec_info(jo, 0);
        /*
        nn=get_next_node(jo);
        while (nn!=0)
        {
            //cout << get_node_name(nn) << endl;
            nodename=get_node_name(nn);
            //print_node_type(nn);
            printf(" %s \n", nodename);
                //printf(" %s\n", get_node_name(nn));
            if (nn->type==json_integer)
            {	//printf(" %Ld \n", nn->u.integer);

            }
            nn=get_next_node(nn);            
        }*/				
    }
}

void xml_info(json_value *jo, int i2)
{
    int k;
 	 int i,c;
 	 json_value *j=jo;
 	 if (jo=0)
                return;
         //for(k=0; k<i2; k++)
           //     printf("\t");               
         if ((j->parent->type)!=json_array)             
                printf("\n<%s>", get_node_name(j));
         else
             printf("\n<element>");
	 switch (j->type)
	 {
	 case json_object :
             i2++;
	  	 //cout << "Dlugosc obiektu: "  << j->u.object.length << endl;
	  	 for(i=0; i<j->u.object.length; i++)
		 {                    
                        xml_info(j->u.object.values[i].value, i2);			
		 }
                i2--;
                //printf("\n");                          
                //for(k=0; k<i2; k++)
                  //      printf("\t");   
                //printf("</%s>", get_node_name(j));
                printf("\n");                
                break;
	 case json_string :
             //utf16_esc_rev(j->u.string.ptr, j->u.string.length, 'u');
             printf("%s", j->u.string.ptr);    
             //printf(" STRING ");
             //printf("</%s>", get_node_name(j));             
		  break;
	 case json_integer :
             printf("%lld", j->u.integer);
             //printf("</%s>", get_node_name(j));             
                break;
	 case json_double :	
             printf("%f", j->u.dbl);
             //printf("</%s>", get_node_name(j));             
		  break;
	 case json_boolean :	
             printf("%s", j->u.boolean ? "TRUE" : "FALSE");
             //printf("</%s>", get_node_name(j));             
		  break;
	 case json_array :
	  	 //cout << "Dlugosc tablicy: "  << j->u.array.length << endl;
		 c=j->u.array.length;
		 //if (c>3) c=3;
	  	 for(i=0; i<c; i++) 		  
			xml_info(j->u.array.values[i], i2);
                 //printf("\n");
		  break;
	 case json_null :	
	 	  //cout << " \"NULL\"  " << endl;
		  break;
		  default :
                      ;
		  //cout << "Not recognize element" << endl;		
	};
        //for(k=0; k<i2; k++)
          //  printf("\t");   
         if ((j->parent->type)!=json_array)             
                printf("</%s>", get_node_name(j));
         else
             printf("</element>"); 
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
    
    SetAttrs(findobj(JM_OBJ_BUTTON_INFO, Win), MUIA_Text_Contents, "parsing... ");
    if (jo!=0)
        json_value_free(jo);
  
  jo=loadJson(nazwa_pliku);
  if (jo)
   {
    //SetAttrs (Info, MUIA_Text_Contents, "parsed ok ");	   
      SetAttrs(findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, infochar);	
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
      SetAttrs(findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, "parser error... ");
} 

APTR *init_font(char *name, int size)
{
    APTR font = 0;
    
    font = TT_OpenFont(
            //TT_FontFile, "PROGDIR:AndaleMo.ttf",
            TT_FontFile, name,
            TT_FontSize, size, 
            TAG_END);
    return font;
}

long FontLoad(Object *filestring reg(a2), Object **listview reg(a1))
{
    FontLoad_noHook(get_font_size_from_menu());
}

long FontLoad_noHook(int size)
{
    char *font_name = 0;
    APTR oldfont = font;
    struct Window *syswin = 0;
    int bl = 0;
    
    if (size==0)
        size = JM_DEFAULT_FONT_SIZE;
    
    GetAttr (MUIA_String_Contents, ttf_string , (long*)&font_name);
    GetAttr(MUIA_Window_Window, Win, &syswin);
    //rp = syswin->RPort;      
    //printf("FOnt name: %s \n", font_name);
    SetAttrs(findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, "Font loading...");    
    //font = 0; 
        SetAttrs (App, MUIA_Application_Sleep, TRUE, TAG_END); 
    font = init_font(font_name, size);
        SetAttrs (App, MUIA_Application_Sleep, FALSE, TAG_END); 
    if (font)
    {        
        if (syswin->RPort)
                TT_SetFont(syswin->RPort, font);
        //printf("OK. Font ready\n");
        SetAttrs(findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, "OK. Font ready");
        TT_SetAttrs(syswin->RPort, TT_Window, (ULONG)syswin, TAG_END);
        //TT_GetAttrs(syswin->RPort, TT_FontBaseline, &bl, TAG_DONE);
        //printf("Baseline: %d\n", bl);
        
        if(oldfont)
                TT_CloseFont(oldfont);  
        MUI_Redraw(ttbitmap_obj, MADF_DRAWOBJECT);        
    }
    else
    {
        font = oldfont;
        ///printf("TTF error...\n");
        SetAttrs(findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, "TTF error...");
    }
}

/* Hook wywo�ywany po wybraniu pliku */

long CzytajPlik (Object *filestring reg(a2), Object **listview reg(a1))
{
    return CzytajPlik_noHook(filestring, *listview);
}
long CzytajPlik_noHook(Object *filestring, Object *listview)
{
  char linia[1024];                  /* miejsce na wczytywane linie */
  char *nazwa_pliku;
  long pozycja, odczyt;
  	//char *nodename;
  //if (Plik) Close (Plik);            /* zamknij ewentualny poprzedni plik */
  DoMethod (listview, MUIM_List_Clear);
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

int convert8utf16(char *utf8, char esc, int *lenchar16) //u must dealloc after use
{
    short *txt16 = 0;
    int txt16len = 0;
    int i = 0;
    
    txt16len = utf_text_info(utf8, esc);
    *lenchar16 = txt16len;
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

  short *txt16 = 0;
  int lenchar = 0;
  
long DoubleClickHook(Object *info reg(a2))
{
  //short *txt16 = 0;
  //int lenchar = 0;
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
        //printf("String UTF8-16 ...\n");
        SetAttrs(ttbitmap_obj, MUIA_UserData, jnode->curjson->u.string.ptr, TAG_END);
        //SetAttrs(ttbitmap_obj, MUIA_Background, MUII_SHADOW, TAG_END);
        txt16 = convert8utf16(jnode->curjson->u.string.ptr, 'u', &lenchar);
        //SetAttrs (Info, MUIA_Text_Contents, "drawing... "); 
        MUI_Redraw(ttbitmap_obj, MADF_DRAWOBJECT);
        //if (txt16) 
            //DoMethod(ttbitmap_obj, MUIM_Draw, MADF_DRAWOBJECT);
            //MUI_Redraw(bitmap_obj, MADF_DRAWOBJECT);        
        /*if (txt16)
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
            TT_Text(rp, txt16, lenchar);
            free(txt16);           
        }*/
        return 0;
  }
  if (jnode->isfolded>0)
	expand(info);
  else
	fold(info);
	//printf(" Klik! :P\n"); 
	return 0;	
}

long close_about(Object* obj,  Msg msg)
{
        SetAttrs (InfoWin, MUIA_Window_Open, FALSE, TAG_END);
}
int setget_test()
{
    int sizeval2;
    //GetAttr(MUIA_UserData, obj, &sizeval);
    GetAttr(TTBM_FONT_SIZE, ttbitmap_obj, &sizeval2);
    //printf("size from menu UserData: %d\n", sizeval);
    printf("size from ttbitmap get: %d\n", sizeval2);    
    //printf("SetAttrs %d \n", SetAttrs(ttbitmap_obj, TTBM_FONT_SIZE, 789, TTBM_FONT_PATH, "test path", TAG_END));
    //printf("SetAttrs %d \n", SetAttrs(ttbitmap_obj, TTBM_FONT_SIZE, 789, TAG_END));
    
    
}

long get_font_size_from_menu()
{
    int i;
    int ischeck = 0;
    Object *mpoz = 0;
    Object *menusize = findobj(JM_OBJ_MENU_SIZE, App);
    
    //printf("get_font_size_from_menu() START\n");
    if (menusize)
        for (i=0; i<5; i++)
	{           
            //printf("get_font_size_from_menu() %d\n", i);
            mpoz = 0; mpoz = findobj(10+(i*2), menusize);
            if (mpoz)
                GetAttr(MUIA_Menuitem_Checked, mpoz, &ischeck);
                if (ischeck == TRUE)
                    return (10+(i*2));
        }
    return 0;
};

long font_size(Object* obj, int *size reg(a1))
{
    long sizeval = *size;
    if(sizeval<10 || sizeval>24)
        sizeval = JM_DEFAULT_FONT_SIZE;
    
    printf("font size change to... %d\n", *size);
        
    FontLoad_noHook(sizeval);       
    //SetAttrs(findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, "font size change to...");
}

long save_xml(Object* obj, Msg msg)
{
    //printf("saving xml... \n");
    SetAttrs (findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, "saving xml... ");     
    //json2xml();
    printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    xml_info(jo, 0);
    SetAttrs (findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, "ok... ");      
}
long test(Object* obj, long *x reg(a1))
{
    int h = 0;
    printf("test hook... line [%d] \n", *x);
    //GetAttr(MUIA_Window_Height, Win, &h);
    //printf("height... %d \n", h);    
    //SetAttrs(ttbitmap_obj, TTBM_FONT_SIZE, 345, TAG_DONE);
    //DoMethod(ttbitmap_obj, MUIM_Set, TTBM_FONT_SIZE, *x, TAG_DONE);
}

long logger_hook(Object* obj, long *x reg(a1))
{
    printf("logger hook: TAG[%s] Value[%d] \n", *x++, *x);
}

int last_selected = 0;

long next_selected(int prev_selected)
{
    long pos = prev_selected;
    //if (pos != MUIV_List_NextSelected_End)
      //  SetAttrs(findobj(JM_OBJ_BTN_SEARCH_NEXT, Win), MUIA_Disabled, FALSE, TAG_END);    
    
    if (pos == MUIV_List_NextSelected_Start)
        DoMethod(Win, MUIM_Set, MUIA_Window_ActiveObject, Listview);
 
    DoMethod(findobj(JM_OBJ_LVIEW_LIST, Listview), MUIM_List_NextSelected, &pos);
    printf("pozycja %d %d\n", pos, prev_selected);
    if (pos != MUIV_List_NextSelected_End)
    {
        DoMethod(findobj(JM_OBJ_LVIEW_LIST, Listview), MUIM_List_Jump, pos);
        SetAttrs(findobj(JM_OBJ_LVIEW_LIST, Listview), MUIA_List_Active, pos, TAG_DONE);
    }
    //else
      //  SetAttrs(findobj(JM_OBJ_BTN_SEARCH_NEXT, Win), MUIA_Disabled, TRUE, TAG_END);        
    return pos;
}

long prev_next_search_hook(Object* obj, int *x reg(a1))
{
    switch (*x)
    {
        case JM_OBJ_BTN_SEARCH_NEXT:
            last_selected = next_selected(last_selected);
            printf(" Next!\n");
            break;
        case JM_OBJ_BTN_SEARCH_PREV:
            setget_test();
            printf(" Prev!\n");
            break;             
    }
}

long search_hook(Object* obj, long *x reg(a1))
{
    int j, i, len = 0;
    json_value *nn = 0;
    json_value *js = jo;
    
    if (*x == 0)
        return 0;
    len = strlen(*x);
    if (len == 0) 
        return 0;
    printf("serach hook: Value[%s] len:%d\n", *x, len);
    
    DoMethod(findobj(JM_OBJ_LVIEW_LIST, Listview), MUIM_List_Select, MUIV_List_Select_All, MUIV_List_Select_Off, NULL);
    //search_all_in_json(js, *x);   
    //nn = js;
    //while (nn!=0)
        //nn = search_next_in_json(nn, *x);
    i = 0;
    j = 0;
    while(i!=-1)
    {
        i = search_next_in_list(i, *x, 1);
        if (i!=-1)
            j++;
    }
    printf("found %d pos\n", j);
    if (j)
    {
        last_selected = next_selected(MUIV_List_NextSelected_Start);
        //SetAttrs(findobj(JM_OBJ_BTN_SEARCH_NEXT, Win), MUIA_Disabled, FALSE, TAG_END);            
    }
}

long search_next_in_list(int p, char *x, int sel)
{
    long i;
    struct json_node_state *jnode = 0;
    json_value *js = 0;
    int len = strlen(x);
    
    for(;;p++)
    {
        DoMethod(findobj(JM_OBJ_LVIEW_LIST, Listview), MUIM_List_GetEntry, p, &jnode);
        if (jnode == NULL)
            return -1;        
        if(jnode->curjson == NULL) 
            return -1;
        else
            js = jnode->curjson;
        if (js->type==json_string)
            if (strncmp(js->u.string.ptr, x, len)==0)
            {
                printf(" %s %d %s\n", get_node_name(js), p, js->u.string.ptr);
                if (sel)
                    DoMethod(findobj(JM_OBJ_LVIEW_LIST, Listview), MUIM_List_Select, p, MUIV_List_Select_On, NULL);
                return ++p;
            }
    }    
    return -1;    
    
}

long get_json_poz_on_list(json_value *js)
{
    int i;
    struct json_node_state *jnode = 0;
    for(i=0;;i++)
    {
        DoMethod(findobj(JM_OBJ_LVIEW_LIST, Listview), MUIM_List_GetEntry, i, &jnode);
        if (jnode == NULL)
            return -1;        
        if(js == jnode->curjson)
            return i;
    }    
    return -1;
}

json_value *search_next_in_json(json_value *js, char *x)
{
    int len = strlen(x);
    json_value *nn=0;
    int d = 0;
    if (js)
    {       
        nn=get_next_node(js);
        while (nn!=0)
        {    
            if (nn->type==json_string)             
                if (strncmp(nn->u.string.ptr, x, len)==0)
                {
                        printf(" %s %d %s\n", get_node_name(nn), get_json_poz_on_list(nn), nn->u.string.ptr);   
                        break;
                }
            nn=get_next_node(nn);
        }
        return get_next_node(nn);
    }
    return 0;
}

long search_all_in_json(json_value *js, char *x)
{
    int len = strlen(x);
    json_value *nn=0;
    int d = 0;
    if (js)
    {       
        nn=get_next_node(js);
        while (nn!=0)
        {    
            if (nn->type==json_string)             
                if (strncmp(nn->u.string.ptr, x, len)==0)                    
                        printf(" %d %s %d %s\n", ++d, get_node_name(nn),  get_json_poz_on_list(nn), nn->u.string.ptr);   
            nn=get_next_node(nn);
        }
    }
    return d;
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
struct Hook h_save_xml = {NULL, NULL, (HOOKFUNC)save_xml, NULL, NULL};
struct Hook h_font_size = {NULL, NULL, (HOOKFUNC)font_size, NULL, NULL};
struct Hook h_test = {NULL, NULL, (HOOKFUNC)test, NULL, NULL};
struct Hook h_logger_hook = {NULL, NULL, (HOOKFUNC)logger_hook, NULL, NULL};
struct Hook h_search_hook = {NULL, NULL, (HOOKFUNC)search_hook, NULL, NULL};
struct Hook h_prev_next_search_hook = {NULL, NULL, (HOOKFUNC)prev_next_search_hook, NULL, NULL};

#define STO150  150
long appMsgHook(Object *info reg(a2), struct AppMessage **appmsg reg(a1))
{
    char lockname[STO150];    
    BPTR oldlock;
    struct AppMessage *apm = *appmsg;
    
        //printf(" app message is here.. %d %s\n", apm->am_NumArgs, apm->am_ArgList[0].wa_Name);
    if (apm->am_NumArgs>0)    
    {
        if (NameFromLock(apm->am_ArgList[0].wa_Lock, lockname, STO150-1))
        {
            printf("AppMessage file: %s %s\n", lockname, apm->am_ArgList[0].wa_Name);
        }
        oldlock = CurrentDir(apm->am_ArgList[0].wa_Lock);
        SetAttrs(String, MUIA_String_Contents, apm->am_ArgList[0].wa_Name, TAG_DONE);
        CzytajPlik_noHook(String, Listview);
        CurrentDir(oldlock);
    }        
}
struct Hook h_appMsgHook = {NULL, NULL, (HOOKFUNC)appMsgHook, NULL, NULL};
// ==================================================

/* Fun GUI */
long BuildApplication (void)
{
    char copyright = 0xA9; 
    char *win_title = 0;
    char *screen_title = 0;
    
    App = MUI_NewObject(MUIC_Application,
        MUIA_Application_Author, (long)JM_App_Author,
        MUIA_Application_Base, (long)JM_App_Base,
        MUIA_Application_Copyright, (long)JM_App_Copyright,
        MUIA_Application_Description, (long)JM_App_Descr,
        MUIA_Application_Title, (long)JM_App_Title,
        MUIA_Application_Version, (long)JM_App_Version,
        /*MUIA_Application_Author, (long)"Tomasz Okoniewski - virago/BlaBla)",
        MUIA_Application_Base, (long)"JSONMASTER",
        MUIA_Application_Copyright, screen_title=(long)"c 2014-2015 by BlaBla Corp.",
        MUIA_Application_Description, (long)"simple json files viewer",
        MUIA_Application_Title, (long)"jsonMaster",
        MUIA_Application_Version, (long)"$VER: jsonMaster 1.0 (21.12.2015) BLABLA PRODUCT",*/
        // ==============================
        MUIA_Application_Menustrip, BuildMenu(),        
        // ==============================        
        MUIA_Application_Window, Win = MUI_NewObject (MUIC_Window,
                MUIA_Window_Title, win_title = (long)"jsonMaster c 2014-2016 by BlaBla group",
                MUIA_Window_ID, 0x50525A4B,
                MUIA_UserData, OBJ_WINDOW,
                MUIA_Window_AppWindow, TRUE,
                MUIA_Window_Height, MUIV_Window_Height_Screen(75),      // remove windowId
                MUIA_Window_Width, MUIV_Window_Width_Screen(50),                
                MUIA_Window_RootObject, MUI_NewObject(MUIC_Group,
                // ttf    ===========================
                MUIA_Group_Child, ttbitmap_obj = NewObject (TTBitmapClass->mcc_Class, NULL,
                        //MUIA_FillArea, FALSE,
                        MUIA_Frame, MUIV_Frame_Text,
                        MUIA_Background, MUII_TextBack,
                        MUIA_FixHeight, 30,       
                        MUIA_UserData, "dup@",
                        MUIA_ShortHelp, (long)" UTF display area... ",
                TAG_END),
                // ==============================                
                MUIA_Group_Child, BuildTTFfontReq(),
                // ==============================================
                MUIA_Group_Child, BuildJsonFileReq(),
                // json file select ^^^^^^
                MUIA_Group_Child, MUI_NewObject (MUIC_Group,       /* grupa z przyciskami */
                        MUIA_Group_Horiz, TRUE,
                        MUIA_Group_Child, create_button("\33cExpand", 'e', JM_OBJ_BUTTON_EXPAND),
                        MUIA_Group_Child, create_button("\33cFold",'f', JM_OBJ_BUTTON_FOLD),
                        MUIA_Group_Child, create_button("\33cExpand All", 0, JM_OBJ_BUTTON_EXALL),
                        MUIA_Group_Child, create_button("\33cFold All", 0, JM_OBJ_BUTTON_FOLDALL),
                TAG_END),
                // ==============================
                MUIA_Group_Child, Listview = BuildListview(),
                // ==============================                
                MUIA_Group_Child, BuildSearchBar(),
                // ==============================                
                MUIA_Group_Child, MUI_NewObject (MUIC_Text,        /* pole informacyjne */
                        MUIA_Frame, MUIV_Frame_Text,
                        MUIA_Background, MUII_TextBack,
                        MUIA_UserData, JM_OBJ_BUTTON_INFO, 
                TAG_END),
                TAG_END),
        TAG_END),
    TAG_END);
    
    SetAttrs(ttbitmap_obj, TTBM_FONT_SIZE, JM_DEFAULT_FONT_SIZE);
    
    win_title[11] = copyright;
    screen_title[0] = copyright;  
    
    InfoWin = 0;    
    InfoWin = BuildInfoWin();
    
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
  int i;  
  //DoMethod(findobj(JM_OBJ_STR_SEARCH, Win), MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,  
    //      findobj(JM_OBJ_BTN_SEARCH_NEXT, Win), 3, MUIM_Set, MUIA_Disabled, FALSE);
  
  //DoMethod(findobj(JM_OBJ_STR_SEARCH, Win), MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,  
    //      findobj(JM_OBJ_BTN_SEARCH_PREV, Win), 3, MUIM_Set, MUIA_Disabled, FALSE);  
  
   DoMethod(findobj(JM_OBJ_BTN_SEARCH_PREV, Win), MUIM_Notify, MUIA_Pressed, TRUE, 
          Win, 3, MUIM_CallHook, &h_prev_next_search_hook, JM_OBJ_BTN_SEARCH_PREV);
   
   DoMethod(findobj(JM_OBJ_BTN_SEARCH_NEXT, Win), MUIM_Notify, MUIA_Pressed, TRUE, 
          Win, 3, MUIM_CallHook, &h_prev_next_search_hook, JM_OBJ_BTN_SEARCH_NEXT);
   
  DoMethod(findobj(JM_OBJ_STR_SEARCH, Win), MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
          Win, 3, MUIM_CallHook, &h_search_hook, MUIV_TriggerValue);
  
        // NOT WORK
  DoMethod(Win, MUIM_Notify, MUIA_Window_Width, MUIV_EveryTime,
        Win, 4, MUIM_CallHook, &h_logger_hook, "MUIA_Window_Width", MUIV_TriggerValue);
  
  //DoMethod(Win, MUIM_Notify, MUIA_Window_Width, MUIV_EveryTime,
    //    Win, 3, MUIM_Set, MUIA_Window_Height, MUIV_TriggerValue);          
  
  for (i=0; i<5; i++)
    DoMethod(findobj((ULONG)(10+(i*2)), App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        findobj((ULONG)(10+(i*2)), App), 3, MUIM_CallHook, &h_font_size, 10+(i*2));
  for (i=0; i<5; i++)
    DoMethod(findobj((ULONG)(10+(i*2)), App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        ttbitmap_obj, 3, MUIM_Set, TTBM_FONT_SIZE, 10+(i*2));
    
  DoMethod(findobj(JM_OBJ_MENU_SAVEXML, App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        App, 2, MUIM_CallHook, &h_save_xml);
  
  DoMethod(findobj(JM_OBJ_LVIEW_LIST, Listview), MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
        ttbitmap_obj, 3, MUIM_CallHook, &h_test, MUIV_TriggerValue);
        //ttbitmap_obj, 3, MUIM_Set, TTBM_FONT_SIZE, MUIV_TriggerValue);
   
  DoMethod(Win, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
          Win, 3, MUIM_CallHook, &h_appMsgHook, MUIV_TriggerValue);       
    
  DoMethod (Win, MUIM_Notify, MUIA_Window_CloseRequest,
        MUIV_EveryTime, App, 2, MUIM_Application_ReturnID,
        MUIV_Application_ReturnID_Quit);
  
   DoMethod(findobj(JM_OBJ_MENU_EXIT, App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
        /* about window */
   DoMethod(findobj(JM_OBJ_MENU_ABOUT, App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        InfoWin, 3, MUIM_Set, MUIA_Window_Open, TRUE);  
   
   DoMethod(findobj(JM_OBJ_MENU_SELTTF, App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        ttf_popup, 1, MUIM_Popstring_Open);     
   
   DoMethod(findobj(JM_OBJ_MENU_OPENFILE, App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        findobj(JM_OBJ_BTN_POPUP_JSON, App), 1, MUIM_Popstring_Open);     
   
   DoMethod (InfoWin, MUIM_Notify, MUIA_Window_CloseRequest, MUIV_EveryTime,
        App, 2, MUIM_CallHook, &h_close_about );
   
   DoMethod (findobj(JM_OBJ_BTN_ABOUT_OK, InfoWin), MUIM_Notify, MUIA_Pressed, TRUE,   
        InfoWin, 3, MUIM_Set, MUIA_Window_Open, FALSE);
   
  /* Notyfikacja na wprowadzenie nazwy pliku */
  DoMethod (ttf_string, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self,
        2, MUIM_CallHook, &h_FontLoad);
  
  DoMethod (String, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self,
        3, MUIM_CallHook, &h_CzytajPlik, Listview);

  DoMethod (findobj(JM_OBJ_BUTTON_EXPAND, Win), MUIM_Notify, MUIA_Pressed,
        FALSE, findobj(JM_OBJ_BUTTON_INFO, App), 2, MUIM_CallHook, &h_ExpandHook);
  
  DoMethod(findobj(JM_OBJ_MENU_EXLEV, App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        findobj(JM_OBJ_BUTTON_INFO, App), 2, MUIM_CallHook, &h_ExpandHook);
     
  DoMethod (findobj(JM_OBJ_BUTTON_FOLD, Win), MUIM_Notify, MUIA_Pressed,
        FALSE, findobj(JM_OBJ_BUTTON_INFO, App), 2, MUIM_CallHook, &h_FoldHook);
  
  DoMethod(findobj(JM_OBJ_MENU_FOLD, App), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        findobj(JM_OBJ_BUTTON_INFO, App), 2, MUIM_CallHook, &h_FoldHook);  
   
  DoMethod (Listview, MUIM_Notify, MUIA_Listview_DoubleClick,
        TRUE, findobj(JM_OBJ_BUTTON_INFO, App), 2, MUIM_CallHook, &h_DoubleClickHook);  

  DoMethod (Listview, MUIM_Notify, MUIA_Listview_ClickColumn,
        MUIV_EveryTime, Win, 3, MUIM_Set, MUIA_Window_ActiveObject, Listview); 
  
  return;
}

/* petla glowna programu */

void MainLoop (void)
 {
  long signals;
  struct Window *syswin = 0;
  struct RastPort *rp = 0;

  DoMethod(App, MUIM_Application_Load, MUIV_Application_Load_ENV);  
  SetAttrs (Win, MUIA_Window_Open, TRUE, TAG_END);
  GetAttr(MUIA_Window_Window, Win, &syswin);
  rp = syswin->RPort;  
  //printf("window: %x\n", syswin);
  //printf("rastport: %x\n", rp);
  
  font = init_font("PROGDIR:AndaleMo.ttf", JM_DEFAULT_FONT_SIZE);
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
  if(font)
      TT_CloseFont(font);
  TT_DoneRastPort(rp);    
  return;
 }
 
/* glowna funkcja programu */
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
                //if(font)
                  //  TT_CloseFont(font);
                //TT_DoneRastPort(rp);              
               }
              else
                  printf("Can't build app error\n");                  
              MUI_DeleteCustomClass (TTBitmapClass);
             }
            else
                printf("Can't create custom class error\n");                
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
 
																            
// =========================================================================================																				


// =========================================================================================																				



// =========================================================================================																				



// =========================================================================================																				


