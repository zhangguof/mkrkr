#ifndef _TWEEN_H
#define _TWEEN_H
#include "tjsNative.h"

//---------------------------------------------------------------------------
// tTJSNC_Tween : TJS tTJSNC_Tween Class
//---------------------------------------------------------------------------
class tTJSNC_Tween : public tTJSNativeClass
{
public:
	tTJSNC_Tween();

	static tjs_uint32 ClassID;

protected:
	tTJSNativeInstance * CreateNativeInstance();
};
//---------------------------------------------------------------------------

extern tTJSNativeClass * TVPCreateNativeClass_Tween();


#endif