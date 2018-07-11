#ifndef _TWEEN_IMPL_H
#define _TWEEN_IMPL_H
#include "tjsNative.h"


//---------------------------------------------------------------------------
// tTJSNI_Tween
//---------------------------------------------------------------------------
class tTJSNI_Tween : public tTJSNativeInstance
{
	typedef tTJSNativeInstance inherited;
	friend class tTVPSoundBufferTimerDispatcher;

public:
	tTJSNI_Tween();
	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param,
			iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();
	double calc(int t,int now,double start,double end,int duration);

};

//---------------------------------------------------------------------------

#endif