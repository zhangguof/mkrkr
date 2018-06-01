#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tjsCommHead.h"
#include "tjsError.h"
// #include "ScriptMgnIntf.h"
#include "Application.h"
#include "DebugIntf.h"
// #include "XP3Archive.h"

#include <locale.h> 



const char* script_str= "Debug.message(\"in startup.tjs test!\");";

tTJS *TVPScriptEngine = NULL;
static bool TVPScriptEngineInit = false;
extern void TVPLoadMessage();

void regist_objects()
{
	// register some TVP classes/objects/functions/propeties
	iTJSDispatch2 *dsp;
	iTJSDispatch2 *global = TVPScriptEngine->GetGlobalNoAddRef();
	tTJSVariant val;


#define REGISTER_OBJECT(classname, instance) \
	dsp = (instance); \
	val = tTJSVariant(dsp/*, dsp*/); \
	dsp->Release(); \
	global->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP, TJS_W(#classname), NULL, \
		&val, global);

	/* classes */
	REGISTER_OBJECT(Debug, TVPCreateNativeClass_Debug());
	// REGISTER_OBJECT(Font, TVPCreateNativeClass_Font());//TODO
	// REGISTER_OBJECT(Layer, TVPCreateNativeClass_Layer());
	// REGISTER_OBJECT(Timer, TVPCreateNativeClass_Timer());
	// REGISTER_OBJECT(AsyncTrigger, TVPCreateNativeClass_AsyncTrigger());
	// REGISTER_OBJECT(System, TVPCreateNativeClass_System());
	// REGISTER_OBJECT(Storages, TVPCreateNativeClass_Storages());
	// REGISTER_OBJECT(Plugins, TVPCreateNativeClass_Plugins());
	// REGISTER_OBJECT(VideoOverlay, TVPCreateNativeClass_VideoOverlay());
	// REGISTER_OBJECT(Clipboard, TVPCreateNativeClass_Clipboard());
	// REGISTER_OBJECT(Scripts, TVPCreateNativeClass_Scripts()); // declared in this file
	// REGISTER_OBJECT(Rect, TVPCreateNativeClass_Rect());
	// REGISTER_OBJECT(Bitmap, TVPCreateNativeClass_Bitmap());
	// REGISTER_OBJECT(ImageFunction, TVPCreateNativeClass_ImageFunction());
	// REGISTER_OBJECT(BitmapLayerTreeOwner, TVPCreateNativeClass_BitmapLayerTreeOwner());

	/* WaveSoundBuffer and its filters */
	// iTJSDispatch2 * waveclass = NULL;
	// REGISTER_OBJECT(WaveSoundBuffer, (waveclass = TVPCreateNativeClass_WaveSoundBuffer()));
	// dsp = new tTJSNC_PhaseVocoder();
	// val = tTJSVariant(dsp);
	// dsp->Release();
	// waveclass->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP|TJS_STATICMEMBER,
	// 	TJS_W("PhaseVocoder"), NULL, &val, waveclass);

	 // Window and its drawdevices 
	// iTJSDispatch2 * windowclass = NULL;
	// REGISTER_OBJECT(Window, (windowclass = TVPCreateNativeClass_Window()));
	// dsp = new tTJSNC_BasicDrawDevice();
	// val = tTJSVariant(dsp);
	// dsp->Release();
	// windowclass->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP|TJS_STATICMEMBER,
	// 	TJS_W("BasicDrawDevice"), NULL, &val, windowclass);

	// Add Extension Classes
	// TVPCauseAtInstallExtensionClass( global );

	// Garbage Collection Hook
	// TVPAddCompactEventHook(&TVPTJSGCCallback);
}


void TVPInitScriptEngine()
{
	if(TVPScriptEngineInit) return;
	TVPScriptEngineInit = true;
	TVPLoadMessage();
	// create script engine object
	
	TVPScriptEngine = new tTJS();
	// add kirikiriz
	TVPScriptEngine->SetPPValue( TJS_W("kirikiriz"), 1 );
	regist_objects();

	// // script system initialization
	
}

int main()
{
	setlocale(LC_ALL, "");

	try
	{
		wprintf(L"start!!\n");

		TVPInitScriptEngine();
		TVPScriptEngine->ExecScript(ttstr(script_str));
	}
	catch(eTJSError &e)
	{
		wprintf(L"eTJSError:%ls\n",e.GetMessage().c_str());
	}


	return 0;
}
