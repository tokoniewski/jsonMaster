/* 
 * File:   ttbitmap.h
 * Author: tokoniewski
 *
 * Created on 21 grudzień 2015, 15:54
 */

#ifndef TTBITMAP_H
#define	TTBITMAP_H

#define reg(a) __asm(#a)

#ifdef	__cplusplus
extern "C" {
#endif
    
struct TTBitmap
 {
  short **txt16;
  int *lenchar;
  struct Window *syswin;
  APTR  cliphandle;  
  short DeltaX;
  short DeltaY;
  //struct MUI_EventHandlerNode EHNode;
 };

__saveds long TTBitmapDispatcher (Class *cl reg(a0), Object *obj reg(a2), Msg msg reg(a1));
//long TTBitmapDispatcher (Class *cl, Object *obj, Msg msg);
        
#ifdef	__cplusplus
}
#endif

#endif	/* TTBITMAP_H */

