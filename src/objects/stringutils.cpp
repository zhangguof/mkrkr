#include "tjsCommHead.h"

#include <deque>
#include <algorithm>
#include <time.h>
#include "stringutils.h"

#include "ScriptMgnIntf.h"

static bool inline TJS_iswspace(tjs_char ch)
{
	// the standard iswspace misses when non-zero page code
	if(ch&0xff00) return false; else return 0!=isspace(ch);
}
//---------------------------------------------------------------------------
static bool inline TJS_iswdigit(tjs_char ch)
{
	// the standard iswdigit misses when non-zero page code
	if(ch&0xff00) return false; else return 0!=isdigit(ch);
}


static bool IsNumber(ttstr _str)
{
	// when str indicates a number, this function converts it to
	// number and put to result, and returns true.
	// otherwise returns false.
	const tjs_char *str = _str.c_str();

	if(!str) return false;
	const tjs_char *orgstr = str;

	if(!*str) return false;
	while(*str && TJS_iswspace(*str)) str++;
	if(!*str) return false;
	if(*str == TJS_W('-')) str++; // sign
	else if(*str == TJS_W('+')) str++, orgstr = str; // sign, but skip
	if(!*str) return false;
	while(*str && TJS_iswspace(*str)) str++;
	if(!*str) return false;
	if(!TJS_iswdigit(*str)) return false;
	while(*str && (TJS_iswdigit(*str) || *str == '.')) str++;
	while(*str && TJS_iswspace(*str)) str++;
	if(*str == 0)
	{
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------


static void RegistToGlobal(iTJSDispatch2* _this,const tjs_char* name);


//---------------------------------------------------------------------------
// tTJSNC_StringUtils
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_StringUtils::ClassID = -1;
tTJSNC_StringUtils::tTJSNC_StringUtils() : tTJSNativeClass(TJS_W("StringUtils"))
{
	TJS_BEGIN_NATIVE_MEMBERS(StringUtils)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*TJS class name*/StringUtils)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/StringUtils)
//----------------------------------------------------------------------

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


//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/isNumber)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;
	*result = IsNumber(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL(/*func. name*/isNumber)

//---------------------------------------------------------------------------

//-- properies
static int test_val = 0;
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

RegistToGlobal(this,TJS_W("isNumber"));
} // end of StringUtils::StringUtils


tTJSNativeInstance *tTJSNC_StringUtils::CreateNativeInstance()
{
	return NULL;
}
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_StringUtils()
{
	tTJSNativeClass *cls = new tTJSNC_StringUtils();
	return cls;
}

static void RegistToGlobal(iTJSDispatch2* _this,const tjs_char* name)
{
	iTJSDispatch2 *global = TVPGetScriptEngine()->GetGlobalNoAddRef();
	tTJSVariant val;
    _this->PropGet(TJS_MEMBERENSURE|TJS_IGNOREPROP, name, NULL, &val,_this);
    global->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP, name, NULL, &val, global);
}