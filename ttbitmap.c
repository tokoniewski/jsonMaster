//#include <proto/exec.h>
//#include <proto/intuition.h>
//#include <proto/muimaster.h>
#include <libraries/mui.h>
//#include <proto/utility.h>
//#include <proto/graphics.h>
//#include <proto/dos.h>
//#include <proto/alib.h>
#include <proto/ttengine.h>
#include <proto/utility.h>
#include <libraries/ttengine.h>
  
#define reg(a) __asm(#a)

#include "ttbitmap.h"
#include "jsonMaster.h"

/* ============== TU ZACZYNA SIË KOD KLASY ====================== */
/* dane obiektu */

/* metoda AskMinMax */
long mAskMinMax (Class *cl, Object *obj, struct MUIP_AskMinMax *msg)
 {
  DoSuperMethodA (cl, obj, (Msg)msg);
  msg->MinMaxInfo->MinWidth += 20;
  msg->MinMaxInfo->DefWidth += 80;
  msg->MinMaxInfo->MaxWidth += 1000;
  msg->MinMaxInfo->MinHeight += 20;
  msg->MinMaxInfo->DefHeight += 50;
  msg->MinMaxInfo->MaxHeight += 800;
  return 0;
 }
 
/* metoda Draw */
long mDraw (Class *cl, Object *obj, struct MUIP_Draw *msg)
{
  //long dx= 20;
  //long dy= 10;
  static char *srctxtptr="dup2";
  int bl = 0;
  struct RastPort *rp = _rp(obj);
  struct TTBitmap *data = INST_DATA(cl,obj);
  struct MUI_RenderInfo *mri = muiRenderInfo(obj);
  
        DoSuperMethodA (cl, obj, msg);
  
  data->cliphandle = 0;
  data->cliphandle = MUI_AddClipping(mri, _left(obj)+2, _mtop(obj)+2, _mright(obj)-_mleft(obj)-2, _mbottom(obj)-_mtop(obj)-2 );
  
  GetAttr(MUIA_UserData, obj, (ULONG *)&srctxtptr);
  data->test=srctxtptr;
  //if (srctxtptr) printf(" User Data %s...\n", srctxtptr);
  //SetAttrs (Info, MUIA_Text_Contents, "drawing... ");  
  if (txt16)
  {
      SetAttrs (findobj(JM_OBJ_BUTTON_INFO, App), MUIA_Text_Contents, "redrawing... ");
        TT_SetAttrs(rp, TT_Window, _window(obj), TAG_END);
        TT_GetAttrs(rp, TT_FontBaseline, &bl, TAG_DONE);              
        SetAPen(rp, 2);
        SetDrMd(rp, JAM1);
        //Move(rp, _mleft(obj)+3, _mtop(obj) + _mheight(obj) - ((_mheight(obj) - bl)/2) );
        Move(rp, _mleft(obj)+3, _mtop(obj) + (_mheight(obj)/2) + (bl/2) );
        TT_SetAttrs(rp,
            TT_Antialias, TT_Antialias_Off, 
            TT_Encoding, TT_Encoding_UTF16_BE, 
            TAG_END);
        //TT_Text(rp, "This is a text printed with TT_Text().", 38);
        TT_Text(rp, txt16, lenchar);
  }
  
  SetAPen (rp, 1);
  Move (rp, _mleft(obj) , _mtop(obj) + (_mheight(obj)/4) );
  Draw (rp, _mright(obj) , _mtop(obj) + (_mheight(obj)/4) );
  //Move (rp, _mleft(obj) , _mtop(obj) + (_mheight(obj)/2) );
  //Draw (rp, _mright(obj) , _mtop(obj) + (_mheight(obj)/2) );
  Move (rp, _mleft(obj) , _mbottom(obj) - (_mheight(obj)/4) );
  Draw (rp, _mright(obj) , _mbottom(obj) - (_mheight(obj)/4) );
  /*
  SetAPen (rp, 2);
  //WritePixel (rp, (_mleft(obj) + _right(obj))/2, _mtop(obj) + 5);
  //WritePixel (rp, _mleft(obj) + 5, (_mtop(obj) + _mbottom(obj))/2);
    */  
  //if (data->cliphandle!=0)      
      MUI_RemoveClipping(mri, data->cliphandle);
  return 0;
}

/* metoda Setup */
long mSetup (Class *cl, Object *obj, Msg msg)
 {
  struct TTBitmap *data = INST_DATA(cl,obj);
  if (DoSuperMethodA (cl, obj, msg))
   {
      /*
    data->EHNode.ehn_Priority = 0;
    data->EHNode.ehn_Flags = 0;
    data->EHNode.ehn_Object = obj;
    data->EHNode.ehn_Class = cl;
    data->EHNode.ehn_Events = IDCMP_MOUSEMOVE;
    DoMethod (_win(obj), MUIM_Window_AddEventHandler, &data->EHNode);
        */
    return TRUE;
   }
  return FALSE;
 }

/* metoda Cleanup */
long mCleanup (Class *cl, Object *obj, Msg msg)
 {
  struct TTBitmap *data = INST_DATA(cl,obj);
    if (DoSuperMethodA (cl, obj, msg))
    {        
    }
  //DoMethod (_win(obj), MUIM_Window_RemEventHandler, &data->EHNode);
 }

/* metoda HandleEvent */
long mHandleEvent (Class *cl, Object *obj, struct MUIP_HandleEvent *msg)
 {
  struct TTBitmap *data = INST_DATA(cl,obj);

  if (msg->imsg)
   {
    if (msg->imsg->Class == IDCMP_MOUSEMOVE)
     {
      //data->DeltaX = msg->imsg->MouseX - _mleft(obj) - 20;
      //data->DeltaY = msg->imsg->MouseY - _mtop(obj) - 20;
      MUI_Redraw (obj, MADF_DRAWOBJECT);
     }
   }
  return 0;
 }

long TTBitmapGet(Class *cl, Object *obj, struct opGet *msg)
{
    struct TTBitmap *data = INST_DATA(cl,obj);
    
    switch (msg->opg_AttrID)
    {
        case TTBM_FONT_SIZE:
            
            *msg->opg_Storage = data->fontsize; 
            return TRUE;
            
        default:
                return DoSuperMethodA (cl, obj, msg);
    }    
}

long TTBitmapSet(Class *cl, Object *obj, struct opSet *msg)
{
    struct TTBitmap *data = INST_DATA(cl,obj);
    struct TagItem *tag, *tagptr;
    int tagcount = 0;
    int fsize = 0;
        
    tagptr = msg->ops_AttrList;
    while ((tag = NextTagItem(&tagptr)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case TTBM_FONT_SIZE:
                //GetAttr(MUIA_UserData, tag->ti_Data, &fsize);
                printf("Setting Font size to %d\n", tag->ti_Data);
                data->fontsize = tag->ti_Data;
                tagcount++;
                break;
            case TTBM_FONT_PATH:
                printf("Setting Font name to %s\n", tag->ti_Data);
                tagcount++;
                break;                
        }
    }
    tagcount += DoSuperMethodA(cl, obj, (Msg)msg);
    return tagcount;
}
               
/* dispatcher */
__saveds long TTBitmapDispatcher (Class *cl reg(a0), Object *obj reg(a2), Msg msg reg(a1))
 {
  switch (msg->MethodID)
   {
    case OM_GET:                return (TTBitmapGet(cl, obj, (struct opGet *)msg));      
    case OM_SET:                return (TTBitmapSet(cl, obj, (struct opSet *)msg));          
    //case OM_SET:                return DoSuperMethodA (cl, obj, msg);
    //case OM_GET:                return DoSuperMethodA (cl, obj, msg);
    case MUIM_Setup:              return (mSetup (cl, obj, msg));
    case MUIM_Cleanup:            return (mCleanup (cl, obj, msg));
    case MUIM_AskMinMax:          return (mAskMinMax (cl, obj, (struct MUIP_AskMinMax*)msg));
    case MUIM_Draw:               return (mDraw (cl, obj, (struct MUIP_Draw*)msg));
    case MUIM_HandleEvent:        return (mHandleEvent (cl, obj, (struct MUIP_HandleEvent*)msg));
    default:                      return (DoSuperMethodA (cl, obj, msg));
   }
 }

/* ============= KONIEC KODU KLASY =============================*/




