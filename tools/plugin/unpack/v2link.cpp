#include <windows.h>
#include "tp_stub.h"
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"
#include <boost/filesystem.hpp>
#include "Archive.hpp"

#define EXPORT(hr) extern "C" __declspec(dllexport) hr __stdcall

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:V2Link=_V2Link@4")
#pragma comment(linker, "/EXPORT:V2Unlink=_V2Unlink@0")
#endif

typedef unsigned char uint8_t; 

void write_file(const char* fname,uint8_t* buf,int size)
{
	FILE *f = NULL;
	fopen_s(&f,fname,"wb");
	if(!f)
		return;
	fwrite(buf,sizeof(uint8_t),size,f);
	fclose(f);
}

void write_file_test()
{
	const char* p = "testtest!";
	write_file("test.log",(uint8_t*)p,strlen(p));
}

extern int write_file(ttstr file_path,ttstr buffer,bool over_write=false);
extern int write_file(ttstr file_path, void* buf, int size, bool over_write=false);

__declspec(dllexport) int WINAPI
DllEntryPoint(HINSTANCE /*hinst*/, unsigned long /*reason*/, void* /*lpReserved*/)
{
	return 1;
}

static tjs_int GlobalRefCountAtInit = 0;

extern bool onV2Link();
extern bool onV2Unlink();

EXPORT(HRESULT) V2Link(iTVPFunctionExporter *exporter)
{
	//MessageBox(NULL,L"v2link1111111!!",L"place",MB_OK);
	TVPInitImportStub(exporter);
	//MessageBox(NULL,L"v2link!!",L"place",MB_OK);

	//write_file_test();
	if (!onV2Link()) return E_FAIL;


	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	
	return S_OK;
}
EXPORT(HRESULT) V2Unlink()
{
	if (TVPPluginGlobalRefCount > GlobalRefCountAtInit ||
		!onV2Unlink()) return E_FAIL;

	TVPUninitImportStub();
	return S_OK;
}

void export_file(ttstr name,ttstr dst_path)
{
	ttstr place(TVPGetPlacedPath(name));
	MessageBox(NULL,place.c_str(),L"place",MB_OK);
	iTJSTextReadStream * stream = TVPCreateTextStreamForRead(place, "");
	ttstr buffer; 
	stream->Read(buffer, 0);
	//write_file("test.tjs",(uint8_t*)buffer.c_str(),buffer.length()*2);
	write_file(dst_path+L"/"+name,buffer,true);
}

extern int unpack_main();
extern bool onV2Link()
{
	AllocConsole();
	//printf("on v2link1111\n");
	wprintf(L"=======on v2link!!!\n");
	try
	{
		unpack_main();
	}
	catch (eTJSError& e)
	{
		wprintf(L"error:%ls\n",e.GetMessage().c_str());
	}
	catch(...)
	{

		wprintf(L"unknow error!!!\n");
	}
	
	//MessageBox(NULL,place.c_str(),L"place",MB_OK);
	//int size = 100 * 1024 * 1024;
	//uint8_t* buf = new uint8_t[size];
	//ULONG cbRead; 
	//s->Read(buf,size,&cbRead);
	
	//char b[100];
	//char* p = itoa(cbRead,b,10);
	//ttstr ss = p;
	//MessageBox(NULL,(ss+L"::after read").c_str(),L"test",MB_OK);
	//write_file(L"data/dtcn.xp3",buf,cbRead,true);
	
	//export_file(L"Initialize.tjs",L"data");

	return true;
}
extern bool onV2Unlink()
{
	return true;
}

