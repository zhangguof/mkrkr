#include "utils.h"
#include "tjs.h"
#include <time.h>
#include "SDL.h"
#include "CharacterSet.h"
#include <boost/filesystem.hpp>

FILE* wfopen(const wchar_t* filename, const wchar_t* mode)  
{  
    char fn[MAX_PATH];  
    char m[MAX_PATH];  
    wcstombs(fn, filename, MAX_PATH);  
    wcstombs(m, mode, MAX_PATH);  
    return fopen(fn, m);  
} 



// 返回自系统开机以来的毫秒数（tick）  
unsigned long GetTickCount()  
{  
    struct timespec ts;  
  
    clock_gettime(CLOCK_MONOTONIC, &ts);  
  
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);  
}

tjs_uint64 TVPGetTickCount()
{
	return (tjs_uint64)GetTickCount();
}

uint32_t timeGetTime()
{
	return GetTickCount();
}


// func not implement
bool TVPGetCommandLine(const tjs_char * name, tTJSVariant *value)
{
	return false;
}



// tjs_int TVPGetThreadNum(void)
// {
// 	return 1;
// }

// void TVPEndThreadTask(void) 
// {
// 	// printf("end thread task\n");
// 	//assert(false);//TODO
// }

// void TVPExecThreadTask(TVP_THREAD_TASK_FUNC func, TVP_THREAD_PARAM param)
// {
// 	// assert(false);//TODO
// 	func(param);
// }
void TVPStartTickCount()
{
	//assert(false);//TODO

}


void TVPMainWindowClosed()
{
	// called from WindowIntf.cpp, caused by closing all window.
	// if( TVPTerminateOnWindowClose) TVPTerminateAsync();
	assert(false);
	//TODO
}

extern ttstr ExePath();
const tjs_char* default_font_name = TJS_W("font/AdobeHeitiStd-Regular.otf");
const tjs_char* default_test_data_path = TJS_W("test-data/");

ttstr TVPGetDefaultFontName() {
	// ttstr name(TJS_W("font/NotoSansCJKsc-Thin.otf"));
	// const tjs_char* font_name = "font/AdobeHeitiStd-Regular.otf";
	ttstr name = default_font_name;
    name = ttstr(TJS_W("file://./")) + GetBundlePath() + name;
	// wprintf(TJS_W("%ls\n"),name.c_str());
	return name;
}

ttstr TVPGetDefaultTestDataPath()
{
    ttstr path = default_test_data_path;
    path = GetBundlePath() + path;
    return path;
}

ttstr TVPGetVersionString()
{
	// TVPGetVersion();
	// tjs_char verstr[100];
	// TJS_snprintf(verstr, sizeof(verstr)/sizeof(tjs_char), TJS_W("%d.%d.%d.%d"),
	// 	TVPVersionMajor, TVPVersionMinor,
	// 	TVPVersionRelease, TVPVersionBuild);
	return ttstr("0.0.0.1");
}

ttstr TVPGetVersionInformation(void)
{
	// TVPGetVersion();
	// tjs_char verstr[100];
	// TJS_snprintf(verstr, sizeof(verstr)/sizeof(tjs_char), TJS_W("%d.%d.%d.%d"),
	// 	TVPVersionMajor, TVPVersionMinor,
	// 	TVPVersionRelease, TVPVersionBuild);

	// tjs_char tjsverstr[100];
	// TJS_snprintf(tjsverstr, sizeof(tjsverstr)/sizeof(tjs_char), TJS_W("%d.%d.%d"),
	// 	TJSVersionMajor, TJSVersionMinor, TJSVersionRelease);

	// ttstr version = TVPFormatMessage(TVPVersionInformation, verstr, tjsverstr);
	// std::wstring str = ApplicationSpecialPath::ReplaceStringAll( version.AsStdString(), std::wstring(L"%DATE%"), std::wstring(TVPCompileDate) );
	// str = ApplicationSpecialPath::ReplaceStringAll( str, std::wstring(L"%TIME%"), std::wstring(TVPCompileTime) );
	return ttstr("Moble  KrKrZ 0.0.0.1");
}

int MulDiv(int a,int b, int c)
{
	long long x = a * b;
	int r = x / c;
	return r;
}


//---------------------------------------------------------------------------
// Versoin retriving
//---------------------------------------------------------------------------
extern void TVPGetVersion();
extern tjs_int TVPVersionMajor;
extern tjs_int TVPVersionMinor;
extern tjs_int TVPVersionRelease;
extern tjs_int TVPVersionBuild;
void TVPGetSystemVersion(tjs_int &major, tjs_int &minor,
	tjs_int &release, tjs_int &build)
{
	TVPGetVersion();
	major = TVPVersionMajor;
	minor = TVPVersionMinor;
	release = TVPVersionRelease;
	build = TVPVersionBuild;
}

//---------------------------------------------------------------------------
void TVPGetTJSVersion(tjs_int &major, tjs_int &minor,
	tjs_int &release)
{
	major = TJSVersionMajor;
	minor = TJSVersionMinor;
	release = TJSVersionRelease;
}
//---------------------------------------------------------------------------
extern ttstr TVPReadAboutStringFromResource();
extern ttstr TVPFormatMessage(const tjs_char *msg, const ttstr & p1, const ttstr & p2);

ttstr TVPGetAboutString(void)
{
	TVPGetVersion();
	tjs_char verstr[100];
	TJS_snprintf(verstr, sizeof(verstr)/sizeof(tjs_char), TJS_W("%d.%d.%d.%d"),
		TVPVersionMajor, TVPVersionMinor,
		TVPVersionRelease, TVPVersionBuild);

	tjs_char tjsverstr[100];
	TJS_snprintf(tjsverstr, sizeof(tjsverstr)/sizeof(tjs_char), TJS_W("%d.%d.%d"),
		TJSVersionMajor, TJSVersionMinor, TJSVersionRelease);

	return TVPFormatMessage(TVPReadAboutStringFromResource().c_str(), verstr, tjsverstr);
}

void TVPGetAllFontList( std::vector<std::wstring>& list )
{
	// list.push_back(TJS_W("font/NotoSansCJKsc-Black.otf"));
	list.push_back(default_font_name);
}

void Sleep(uint32_t ms)
{
	SDL_Delay(ms);
}

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
extern const char* ios_get_data_path();
#endif

ttstr GetAppPath()
{
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    static ttstr path = ios_get_data_path();
#else
    static ttstr path = ExePath();
#endif
    if(path.GetLastChar()!=TJS_W('/'))
        path += TJS_W('/');
    SDL_Log("GetAppPath:%s",path.AsNarrowStdString().c_str());
    
    return path;
}

ttstr GetAppDocumentsPath()
{
    return GetAppPath();
}
//like exepath
ttstr GetBundlePath()
{
    auto cur_path = boost::filesystem::current_path();
    ttstr p(cur_path.c_str());
    if(p.GetLastChar()!=TJS_W('/'))
        p = p + TJS_W('/');
    return p;
}

// int MultiByteToWideChar(
//   UINT                              CodePage,
//   DWORD                             dwFlags,
//   _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
//   int                               cbMultiByte,
//   LPWSTR                            lpWideCharStr,
//   int                               cchWideChar
// );
int MultiByteToWideChar(uint32_t CodePage,int dwFlags,const char* lpMultiByteStr,
						int cbMultiByte,wchar_t* lpWideCharStr,int cchWideChar)
{
	assert(CodePage == CP_UTF8);
	int r = 0;
	r = TVPUtf8ToWideCharString(lpMultiByteStr,lpWideCharStr);
	return r;
}

// __int64 _wcstoi64(
//    const wchar_t *strSource,
//    wchar_t **endptr,
//    int base
// );
int64_t _wcstoi64(const wchar_t* strSource, wchar_t **endptr,int base)
{
	return std::wcstoll(strSource,endptr,base);
}
