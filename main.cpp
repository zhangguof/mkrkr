#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tjsCommHead.h"
#include "tjsError.h"
#include "ScriptMgnIntf.h"
#include "Application.h"
#include "DebugIntf.h"
#include "StorageIntf.h"
#include "XP3Archive.h"
#include "SysInitIntf.h"

#include <locale.h> 



const char* script_str= "Debug.message(\"in startup.tjs test!\");";

extern tTJS *TVPScriptEngine;
static bool TVPScriptEngineInit = false;
extern void TVPLoadMessage();
// extern void TVPSystemInit();

// TVPInitScriptEngine

extern void TVPSetCurrentDirectory(const ttstr & _name);

void xp3_filter(tTVPXP3ExtractionFilterInfo* info)
{
	// assert(info->SizeOfSelf == 0x18);
	tjs_uint8 * buff = (tjs_uint8*) (info->Buffer);
	tjs_uint32 file_hash = info->FileHash;
	tjs_uint8 h = file_hash & 0xff;
	for(tjs_uint i=0;i< info->BufferSize;++i)
	{
		tjs_uint8 a = buff[i];
		a = a ^ (h + 1);
		a = ~a;
		buff[i] = a;
	}
}

extern void TVPInitImportFuncs();

extern "C" int app_main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	// TVPSetCurrentDirectory(ExePath());
	// char buf[1024];
	// tTJSBinaryStream *st = TVPCreateStream(ttstr("README"),TJS_BS_READ);
	// st->Read(buf,st->GetSize());
	// buf[st->GetSize()] = '\0';
	// ttstr code(buf);
	// wprintf(TJS_W("size:%d,%ls\n"),st->GetSize(),code.c_str());
	Application = new tTVPApplication;
	TVPInitImportFuncs();
	TVPLoadMessage();

	TVPSetXP3ArchiveExtractionFilter(xp3_filter);
	try
	{
		wprintf(L"start!!\n");
		Application->StartApplication(argc,argv);
		TVPSystemInit();
		TVPInitializeStartupScript();
		Application->Run();
	}
	catch(eTJSError &e)
	{
		Application->PrintConsole(e.GetMessage().c_str(),e.GetMessage().GetLen());
	}


	return 0;
}
