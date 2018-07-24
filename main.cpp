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
#include "SDL.h"
#include <boost/filesystem.hpp>



const char* script_str= "Debug.message(\"in startup.tjs test!\");";

extern tTJS *TVPScriptEngine;
static bool TVPScriptEngineInit = false;
extern void TVPLoadMessage();
// extern void TVPSystemInit();

// TVPInitScriptEngine

extern void TVPSetCurrentDirectory(const ttstr & _name);

// hash, offest buffer length
// Storages.setXP3ArchiveExtractionFilter(function(h,o,b,l){
// 	var k = (h ^ 0x2A09A745) & 0x7FFFFFFF;
// 	k |= k << 31; k &= 0xFFFFFFFF;
// 	var t = []; t[31] = 0;
// 	for(var i = 0; i < 32; ++i) {
// 		t[i] = k;
// 		k = (k << 23) | (k >> 8);
// 		k &= 0xFFFFFFFF;
// 	}

// 	for(var i = 0; i < l; ++i) {
// 		b[i] ^= t[(o + i) & 31];
// 	}
// });

void xp3_filter2(tTVPXP3ExtractionFilterInfo* info)
{
	// assert(info->SizeOfSelf == 0x18);
	tjs_uint8 * buff = (tjs_uint8*) (info->Buffer);
	tjs_uint32 h = info->FileHash;
	tjs_uint32 length = info->BufferSize;
	tjs_uint64 offest = info->Offset;
	tjs_uint32 k = (h ^ 0x2A09A745) & 0x7FFFFFFF;
	k |= k << 31;
	k &= 0xFFFFFFFF;

    tjs_uint32 t[32];
	t[31] = 0;

	for(int i = 0; i < 32; ++i) {
		t[i] = k;
		k = (k << 23) | (k >> 8);
		k &= 0xFFFFFFFF;
	}

	for(int i = 0; i < length; ++i) {
		buff[i] ^= t[(offest + i) & 31];
	}

}

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

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
extern void path_info();
extern void init_locale();
#else
void path_info(){}
void init_locale()
{
    setlocale(LC_CTYPE,"UTF-8");
}
#endif

extern "C" int app_main(int argc, char* argv[])
{
//    setlocale(LC_ALL, "");
//    setlocale(LC_CTYPE,"UTF-8");
    init_locale();
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

	TVPSetXP3ArchiveExtractionFilter(xp3_filter2);
	try
	{
		wprintf(L"start!!\n");
//        boost::filesystem::path p(boost::filesystem::current_path().string() + "/data/startup.tjs");
//        auto tt= boost::filesystem::exists(p);
//        SDL_Log("exist??:%d",tt);
//        char *base_path = SDL_GetPrefPath("tony", "com.tony.mkrkr");
//        if(base_path)
//        {
//            SDL_Log("base path:%s",base_path);
//        }
       
//        SDL_Log("cur path:%s",SDL_GetBasePath());
//         std::string base = base_path;
//        boost::filesystem::path pp(base+"DATA/startup.tjs");
//        auto ret = boost::filesystem::exists(pp);
//        SDL_Log("is exist:%s?%d",pp.c_str(),ret);
        path_info();
        
        
		Application->StartApplication(argc,argv);
		TVPSystemInit();
		TVPInitializeStartupScript();
		Application->Run();
	}
	catch(eTJSError &e)
	{
		Application->PrintConsole(e.GetMessage(),e.GetMessage().GetLen());
	}


	return 0;
}


