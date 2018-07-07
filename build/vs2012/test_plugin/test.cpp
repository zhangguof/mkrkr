#include <string>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv)
{
	_TCHAR* path = L"F:\\BaiduYunDownload\\ħ��ʹ֮ҹ\\plugin\\fstat.dll";
	auto h = LoadLibrary(path);
	if(!h)
	{
		auto dw = GetLastError();
		printf("error:%d\n",dw);
	}
	return 0;
}