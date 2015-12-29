//#include <proto/exec.h>
//#include <proto/intuition.h>
#include <proto/muimaster.h>
//#include <proto/utility.h>
//#include <proto/graphics.h>
//#include <proto/dos.h>
//#include <proto/alib.h>

#define reg(a) __asm(#a)

#include "ttbitmap.h"

/* ============== TU ZACZYNA SIË KOD KLASY ====================== */
/* dane obiektu */

/* metoda AskMinMax */

long mAskMinMax (Class *cl, Object *obj, struct MUIP_AskMinMax *msg)
 {
  DoSuperMethodA (cl, obj, (Msg)msg);
  msg->MinMaxInfo->MinWidth += 20;
  msg->MinMaxInfo->DefWidth += 80;
  msg->MinMaxInfo->MaxWidth += 3000;
  msg->MinMaxInfo->MinHeight += 20;
  msg->MinMaxInfo->DefHeight += 30;
  msg->MinMaxInfo->MaxHeight += 2000;
  return 0;
 }

/* metoda Draw */

#define RADIUS 18   /* promieď strzaîki */

long mDraw (Class *cl, Object *obj, struct MUIP_Draw *msg)
 {
  short big_radius;
  long dx= 20;
  long dy= 10;
  struct RastPort *rp = _rp(obj);
  struct TTBitmap *data = INST_DATA(cl,obj);

  DoSuperMethodA (cl, obj, (Msg)msg);
 
  SetAPen (rp, 1);
  Move (rp, _mleft(obj) + 5, _mtop(obj) + 5);
  Draw (rp, _mright(obj) - 5, _mbottom(obj) - 5);
  Move (rp, _mleft(obj) + 5, _mbottom(obj) - 5);
  Draw (rp, _mright(obj) - 5, _top(obj) + 5);
  
  SetAPen (rp, 2);
  //WritePixel (rp, (_mleft(obj) + _right(obj))/2, _mtop(obj) + 5);
  //WritePixel (rp, _mleft(obj) + 5, (_mtop(obj) + _mbottom(obj))/2);
  Move (rp, (_mleft(obj) + _mright(obj))/2, _mtop(obj) + 5);
  Draw (rp, (_mleft(obj) + _mright(obj))/2, _bottom(obj) - 5);    
  Move (rp, _mleft(obj)+ 5, (_mtop(obj) + _mbottom(obj))/2);
  Draw (rp, _mright(obj) - 5, (_mtop(obj) + _mbottom(obj))/2);
    
  printf("mDraw...\n");
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
      data->DeltaX = msg->imsg->MouseX - _mleft(obj) - 20;
      data->DeltaY = msg->imsg->MouseY - _mtop(obj) - 20;
      MUI_Redraw (obj, MADF_DRAWOBJECT);
     }
   }
  return 0;
 }

/* dispatcher */

__saveds long TTBitmapDispatcher (Class *cl reg(a0), Object *obj reg(a2), Msg msg reg(a1))
 {
  switch (msg->MethodID)
   {
    case MUIM_Setup:              return (mSetup (cl, obj, msg));
    case MUIM_Cleanup:            return (mCleanup (cl, obj, msg));
    case MUIM_AskMinMax:          return (mAskMinMax (cl, obj, (struct MUIP_AskMinMax*)msg));
    case MUIM_Draw:               return (mDraw (cl, obj, (struct MUIP_Draw*)msg));
    case MUIM_HandleEvent:        return (mHandleEvent (cl, obj, (struct MUIP_HandleEvent*)msg));
    default:                      return (DoSuperMethodA (cl, obj, msg));
   }
 }

/* ============= KONIEC KODU KLASY =============================*/

