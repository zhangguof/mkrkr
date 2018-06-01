#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#include "tjs.h"
#include "tjsError.h"

#include <locale.h>

const char* script_str= "Debug.message(\"in startup.tjs test!\");";

tTJS *TVPScriptEngine = NULL;
static bool TVPScriptEngineInit = false;
extern void TVPLoadMessage();
void TVPInitScriptEngine()
{
	if(TVPScriptEngineInit) return;
	TVPScriptEngineInit = true;
	// create script engine object
	TVPLoadMessage();
	TVPScriptEngine = new tTJS();

	// add kirikiriz
	TVPScriptEngine->SetPPValue( TJS_W("kirikiriz"), 1 );

	// // script system initialization
	
}

int main()
{
	// TVPInitScriptEngine();
	// TVPScriptEngine->ExecScript(ttstr(script_str));
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