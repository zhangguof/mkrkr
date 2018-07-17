#include "tjsCommHead.h"

#include <deque>
#include <algorithm>
#include <time.h>
#include "stringutils.h"

#include "ScriptMgnIntf.h"
#include "tjsArray.h"
#include "MsgIntf.h"

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


//change from script


//    for(var i=0; i<keys.count; i++)
//    {
//        //        keys[i]        = keys[i].split(",");
//        var    key    = keys[i], j = 0;
//        var    array = keys[i] = [];
//        while(1)
//        {
//            var    pos_c    = key.indexOf(",");
//            var    pos_b    = key.indexOf("(");
//            //            dm("key = "+key+" / comma = "+pos_c+" / bracket = "+pos_b);
//            if(pos_c < 0)
//            {
//                array[j]    += key;
//                break;
//            }
//            else if(pos_b < 0 || pos_c < pos_b)
//            {
//                array[j]+= key.substr(0, pos_c);
//                key        = key.substr(pos_c + 1);
//                j++;
//            }
//            else
//            {
//                pos_b    = key.indexOf(")");
//                //                dm("bracket = "+pos_b);
//                array[j]+= key.substr(0, pos_b+1);
//                key        = key.substr(pos_b + 1);
//            }
//        }
//        keys[i][0]    = +keys[i][0];
//    }
//    return keys;
//}

static void parseKeyFrame(const ttstr& framekeys,tTJSVariant* result)
{
    printf("test in parseKeyFrame!:%ls\n",framekeys.c_str());
    
    //    if(keyframe[0] != "(")
    //        throw new Exception("キーフレームが解析できません。: "+keyframe);
    //    var    keys    = [];
    //    var    pos    = keyframe.indexOf(")(");
    //    while(pos >= 0)
    //    {
    //        keys.add(keyframe.substr(0, pos));
    //        keyframe    = keyframe.substr(pos+2);
    //        pos    = keyframe.indexOf(")(");
    //    }
    //    if(keyframe[keyframe.length - 1] != ")")
    //        throw new Exception("キーフレームの最後が不正です: "+keyframe);
    //    keys.add(keyframe.substr(0, keyframe.length - 1));
    if(framekeys.GetLastChar()!=TJS_W(')'))
    {
       ttstr msg = TJS_W("キーフレームの最後が不正です: ");
       msg += framekeys;
       TVPThrowExceptionMessage(msg.c_str());
       return;
    }
    const tjs_char* p = framekeys.c_str();
    int len = framekeys.length();
    const tjs_char* e = p+len;
    
    
    if(*p!=TJS_W('('))
    {
       ttstr msg = TJS_W("キーフレームが解析できません。: ");
       msg += framekeys;
       TVPThrowExceptionMessage(msg.c_str());
       return;
    }
    p++;
    
    
    std::vector<ttstr> keys;
    
    
    
    const tjs_char* spos = TJS_strstr(p,TJS_W(")("));
    while(spos)
    {
        keys.push_back(ttstr(p,spos-p));
        p = spos + 2;
        spos = TJS_strstr(p,TJS_W(")("));
    }
    keys.push_back(ttstr(p,e-p-1));
    
    printf("has push %d key\n",keys.size());
    
    
    iTJSDispatch2 *array = TJSCreateArrayObject();
    tTJSVariant tmp(array, array);
    array->Release();
    
    
    
    int cnt = 0;
    for(int i=0;i<keys.size();++i)
    {
        tTJSVariant t = keys[i];
        array->PropSetByNum(TJS_MEMBERENSURE, cnt++,
                            &t, array);
    }

//    array->PropSetByNum(TJS_MEMBERENSURE, cnt++, &tTJSVariant(12), array);
    
    if(result)
        *result = tmp;
}


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
    
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/parseKeyFrame)
    {
        if(numparams<1) return TJS_E_BADPARAMCOUNT;
//        *result = IsNumber(*param[0]);
        
        if(param[0]->Type() == tvtObject)
        {
            iTJSDispatch2 *obj = param[0]->AsObject();
            if(obj && obj->IsInstanceOf(TJS_IGNOREPROP,NULL,NULL,L"Array",obj) == TJS_S_TRUE)
            {
                if(result)
                    *result = *param[0];
                return TJS_S_OK;
            }
        }
        parseKeyFrame(*param[0],result);
        
        return TJS_S_OK;
    }
TJS_END_NATIVE_STATIC_METHOD_DECL(/*func. name*/parseKeyFrame)
    
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
