//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// System Initialization and Uninitialization
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include <vector>
#include <algorithm>
#include <functional>

#include "tjsUtils.h"
#include "SysInitIntf.h"
#include "ScriptMgnIntf.h"
// #include "tvpgl.h"
#include "utils/Exception.h"
#include "MsgImpl.h"
#include "MsgImpl.h"
#include <boost/filesystem.hpp>


//---------------------------------------------------------------------------
// global data
//---------------------------------------------------------------------------
ttstr TVPProjectDir; // project directory (in unified storage name)
ttstr TVPDataPath; // data directory (in unified storage name)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// global data
//---------------------------------------------------------------------------
std::wstring TVPNativeProjectDir;
std::wstring TVPNativeDataPath;
bool TVPProjectDirSelected = false;

extern bool TVPCheckExistentLocalFile(const ttstr &name);
extern bool TVPCheckExistentLocalFolder(const ttstr &name);
extern ttstr ExePath();
extern void TVPSetCurrentDirectory(const ttstr & _name);
extern ttstr TVPNormalizeStorageName(const ttstr & _name);
extern tjs_char  TVPArchiveDelimiter;
extern void TVPAddImportantLog(const ttstr &line);

void sel_project_dir()
{
	wchar_t buf[MAX_PATH];
	bool bufset = false;
	bool nosel = false;
	bool forcesel = false;
	bool forcedataxp3 = false;

	// check "data.xp3" archive
 	if(!nosel)
	{
		wchar_t tmp[MAX_PATH];
		TJS_strcpy(tmp,ExePath().c_str());
		TJS_strcat(tmp, TJS_W("data.xp3"));
		if(TVPCheckExistentLocalFile(ttstr(tmp)))
		{
			TJS_strcpy(buf, tmp);
			TVPProjectDirSelected = true;
			bufset = true;
			nosel = true;
		}
	}

	// check "data.exe" archive
 // 	if(!nosel)
	// {
	// 	wchar_t tmp[MAX_PATH];
	// 	TJS_strcpy(tmp, IncludeTrailingBackslash(ExtractFileDir(ExePath())).c_str());
	// 	TJS_strcat(tmp, TJS_W("data.exe"));

	// 	if(FileExists(tmp))
	// 	{
	// 		TJS_strcpy(buf, tmp);
	// 		TVPProjectDirSelected = true;
	// 		bufset = true;
	// 		nosel = true;
	// 	}
	// }

	// check self combined xpk archive
	// if(!nosel)
	// {
	// 	if(TVPIsXP3Archive(TVPNormalizeStorageName(ExePath())))
	// 	{
	// 		TJS_strcpy(buf, ExePath().c_str());
	// 		TVPProjectDirSelected = true;
	// 		bufset = true;
	// 		nosel = true;
	// 	}
	// }


	// check "data" directory
	if(!forcedataxp3 && !nosel)
	{
		wchar_t tmp[MAX_PATH];
		TJS_strcpy(tmp, ExePath().c_str());
		TJS_strcat(tmp, TJS_W("data"));
		if(TVPCheckExistentLocalFolder(ttstr(tmp)))
		{
			TJS_strcat(tmp, TJS_W("/"));
			TJS_strcpy(buf, tmp);
			TVPProjectDirSelected = true;
			bufset = true;
			nosel = true;
		}
	}

	// decide a directory to execute or to show folder selection
	if(!bufset)
	{
		if(forcedataxp3) throw EAbort(TJS_W("Aborted"));
		TJS_strcpy(buf, ExePath().c_str());
		int curdirlen = (int)TJS_strlen(buf);
		if(buf[curdirlen-1] != TJS_W('/')) buf[curdirlen] = TJS_W('/'), buf[curdirlen+1] = 0;
	}

	// check project dir and store some environmental variables
	// if(TVPProjectDirSelected)
	// {
	// 	Application->SetShowMainForm( false );
	// }

	tjs_int buflen = (tjs_int)TJS_strlen(buf);
	if(buflen >= 1)
	{
		if(buf[buflen-1] != TJS_W('/')) buf[buflen] = TVPArchiveDelimiter, buf[buflen+1] = 0;
	}

	TVPProjectDir = TVPNormalizeStorageName(buf);
	TVPSetCurrentDirectory(TVPProjectDir);
	TVPNativeProjectDir = buf;

	if(TVPProjectDirSelected)
	{
		TVPAddImportantLog( TVPFormatMessage(TVPInfoSelectedProjectDirectory, TVPProjectDir) );
	}
}


extern void TVPGL_C_Init();
extern void TVPInitTVPGL();
//---------------------------------------------------------------------------
// TVPSystemInit : Entire System Initialization
//---------------------------------------------------------------------------
void TVPSystemInit(void)
{
	// TVPBeforeSystemInit();
	TVPSetCurrentDirectory(ExePath());
	sel_project_dir();
	TVPInitScriptEngine();
//TODO
	TVPInitTVPGL();
	TVPGL_C_Init();

	// TVPAfterSystemInit();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TVPSystemUninit : System shutdown, cleanup, etc...
//---------------------------------------------------------------------------
static void TVPCauseAtExit();
bool TVPSystemUninitCalled = false;
void TVPSystemUninit(void)
{
	if(TVPSystemUninitCalled) return;
	TVPSystemUninitCalled = true;

	// TVPBeforeSystemUninit();

	// TVPUninitTVPGL();

	try
	{
		TVPUninitScriptEngine();
	}
	catch(...)
	{
		// ignore errors
	}

//TODO
	// TVPAfterSystemUninit();

	TVPCauseAtExit();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPAddAtExitHandler related
//---------------------------------------------------------------------------
struct tTVPAtExitInfo
{
	tTVPAtExitInfo(tjs_int pri, void(*handler)())
	{
		Priority = pri, Handler = handler;
	}

	tjs_int Priority;
	void (*Handler)();

	bool operator <(const tTVPAtExitInfo & r) const
		{ return this->Priority < r.Priority; }
	bool operator >(const tTVPAtExitInfo & r) const
		{ return this->Priority > r.Priority; }
	bool operator ==(const tTVPAtExitInfo & r) const
		{ return this->Priority == r.Priority; }

};
static std::vector<tTVPAtExitInfo> *TVPAtExitInfos = NULL;
static bool TVPAtExitShutdown = false;
//---------------------------------------------------------------------------
void TVPAddAtExitHandler(tjs_int pri, void (*handler)())
{
	if(TVPAtExitShutdown) return;

	if(!TVPAtExitInfos) TVPAtExitInfos = new std::vector<tTVPAtExitInfo>();
	TVPAtExitInfos->push_back(tTVPAtExitInfo(pri, handler));
}
//---------------------------------------------------------------------------
static void TVPCauseAtExit()
{
	if(TVPAtExitShutdown) return;
	TVPAtExitShutdown = true;

	std::sort(TVPAtExitInfos->begin(), TVPAtExitInfos->end()); // descending sort

	std::vector<tTVPAtExitInfo>::iterator i;
	for(i = TVPAtExitInfos->begin(); i!=TVPAtExitInfos->end(); i++)
	{
		i->Handler();
	}

	delete TVPAtExitInfos;
}
//---------------------------------------------------------------------------






