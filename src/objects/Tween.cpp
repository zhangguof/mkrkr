#include "tjsCommHead.h"

#include <deque>
#include <algorithm>
#include <time.h>
#include "Tween.hpp"
#include "TweenImpl.hpp"

#include "ScriptMgnIntf.h"


static void RegistToGlobal(iTJSDispatch2* _this,const tjs_char* name);

static int test_val = 0;
//---------------------------------------------------------------------------
// tTJSNC_Tween
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_Tween::ClassID = -1;
tTJSNC_Tween::tTJSNC_Tween() : tTJSNativeClass(TJS_W("Tween"))
{
	TJS_BEGIN_NATIVE_MEMBERS(Tween)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var.name*/_this, /*var.type*/tTJSNI_Tween,
	/*TJS class name*/Tween)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Tween)

//-- methods

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/test)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;
	ttstr msg = *param[0];
	wprintf(TJS_W("======:%ls\n"),msg.c_str());

	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL(/*func. name*/test)

//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/calc)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Tween);
	if(numparams < 5) return TJS_E_BADPARAMCOUNT;

	if(result) *result = _this->calc(*param[0],*param[1],*param[2],
									*param[3],*param[4]);
	// printf("do calc!!");

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/calc)



//---------------------------------------------------------------------------

//-- properies

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(val)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = test_val;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		test_val = *param;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_STATIC_PROP_DECL(val)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS

// RegistToGlobal(this,TJS_W("tween"));
} // end of Tween::Tween


tTJSNativeInstance *tTJSNC_Tween::CreateNativeInstance()
{
	return new tTJSNI_Tween();
}
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_Tween()
{
	tTJSNativeClass *cls = new tTJSNC_Tween();
	return cls;
}

static void RegistToGlobal(iTJSDispatch2* _this,const tjs_char* name)
{
	iTJSDispatch2 *global = TVPGetScriptEngine()->GetGlobalNoAddRef();
	tTJSVariant val;
    _this->PropGet(TJS_MEMBERENSURE|TJS_IGNOREPROP, name, NULL, &val,_this);
    global->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP, name, NULL, &val, global);
}