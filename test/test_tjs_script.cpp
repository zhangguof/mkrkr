#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tjsCommHead.h"
#include <locale.h>

const char* script_str= "Debug.message(\"in startup.tjs test!\");";

tTJS *TVPScriptEngine = NULL;
static bool TVPScriptEngineInit = false;
void TVPInitScriptEngine()
{
	if(TVPScriptEngineInit) return;
	TVPScriptEngineInit = true;
	// create script engine object
	TVPScriptEngine = new tTJS();

	// add kirikiriz
	TVPScriptEngine->SetPPValue( TJS_W("kirikiriz"), 1 );

	// // script system initialization
	// TVPScriptEngine->ExecScript(ttstr(TVPInitTJSScript));
}

int main()
{
	TVPInitScriptEngine();
	

	return 0;
}