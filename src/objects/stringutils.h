#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H
#include "tjsNative.h"

//---------------------------------------------------------------------------
// tTJSNC_StringUtils : TJS StringUtils Class
//---------------------------------------------------------------------------
class tTJSNC_StringUtils : public tTJSNativeClass
{
public:
	tTJSNC_StringUtils();

	static tjs_uint32 ClassID;

protected:
	tTJSNativeInstance * CreateNativeInstance();
};
//---------------------------------------------------------------------------

extern tTJSNativeClass * TVPCreateNativeClass_StringUtils();


#endif