#include "utils.h"
#include <time.h>  

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


// func not implement
bool TVPGetCommandLine(const tjs_char * name, tTJSVariant *value)
{
	return false;
}



tjs_int TVPGetThreadNum(void)
{
	return 1;
}

void TVPEndThreadTask(void) 
{
	// printf("end thread task\n");
	//assert(false);//TODO
}

void TVPExecThreadTask(TVP_THREAD_TASK_FUNC func, TVP_THREAD_PARAM param)
{
	// assert(false);//TODO
	func(param);
}
void TVPStartTickCount()
{
	assert(false);//TODO

}
void TVPBeginThreadTask(tjs_int taskNum)
{
	// printf("start thread task!\n");
	// assert(false);//TODO
}
void TVPGetFontRasterizer()
{
	assert(false);//TODO
}

void TVPSetFontRasterizer( tjs_int index )
{
	assert(false);//TODO
}

void TVPMainWindowClosed()
{
	// called from WindowIntf.cpp, caused by closing all window.
	// if( TVPTerminateOnWindowClose) TVPTerminateAsync();
	assert(false);
	//TODO
}

const tjs_char *TVPGetDefaultFontName() {
	return TJS_W("test");
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
	return ttstr("moblie krkrz 0.0.0.1");
}
