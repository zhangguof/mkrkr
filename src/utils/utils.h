#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdio.h>
#include "tjsCommHead.h"
#include "win_def.h"


FILE* wfopen(const wchar_t* filename, const wchar_t* mode);
#define _wfopen wfopen

extern unsigned long GetTickCount();
extern tjs_uint64 TVPGetTickCount();
extern void TVPStartTickCount();
extern uint32_t timeGetTime();

extern bool TVPGetCommandLine(const tjs_char * name, tTJSVariant *value);


typedef void (TJS_USERENTRY *TVP_THREAD_TASK_FUNC)(void *);
typedef void * TVP_THREAD_PARAM;
int MulDiv(int a,int b, int c);

// const tjs_int TVPMaxThreadNum;
extern void TVPBeginThreadTask(tjs_int taskNum);
extern void TVPEndThreadTask(void);
extern tjs_int TVPGetThreadNum(void);
extern void TVPExecThreadTask(TVP_THREAD_TASK_FUNC func, TVP_THREAD_PARAM param);

void Sleep(uint32_t ms);

ttstr GetAppPath();
extern int MultiByteToWideChar(uint32_t CodePage,int dwFlags,const char* lpMultiByteStr,
						int cbMultiByte,wchar_t* lpWideCharStr,int cchWideChar);

#define CP_UTF8 (65001)
#define CP_ACP  (CP_UTF8)

extern int64_t _wcstoi64(const wchar_t* strSource, wchar_t **endptr,int base);

#endif
