#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/error.h>

#include "tjsCommHead.h"
#include "ScriptMgnIntf.h"
#include "Application.h"
#include "XP3Archive.h"
#include <boost/filesystem.hpp>
#include <vector>
#include <codecvt>

#include <locale.h> 

class tTVPTextReadStream;

extern void TVPLoadMessage();
extern void TVPSystemInit(void);
extern iTJSTextReadStream * TVPCreateTextStreamForReadWithStream(const ttstr & name,
	const ttstr & modestr, tTJSBinaryStream* s);

const char* script_str= "Debug.message(\"in startup.tjs test!\");";

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

int write_file(ttstr file_path, void* buf, int size, bool over_write=false)
{
	boost::filesystem::path p(file_path.c_str());
	auto pp = p.parent_path();
	if(!over_write && boost::filesystem::exists(p))
	{
		wprintf(L"exist file:%d,skip:%ls\n",over_write,file_path.c_str());
		return 0;
	}
	if(!boost::filesystem::exists(pp))
	{
		if(!(boost::filesystem::create_directories(pp)))
		{
			wprintf(L"create_directories error:%ls\n",pp.c_str());
			return -1;
		}
	}
	// auto f  = ::fopen(file_path.AsNarrowStdString().c_str(),"wb");
	auto f  = ::fopen(p.c_str(),"wb");
	if(f==0)
	{
		wprintf(L"open file:%ls error:%d\n",file_path.c_str(),errno);
		printf("err:%s\n", strerror(errno));
		return -1;
	}
	int ret = ::fwrite(buf,sizeof(char),size, f);
	if(ret)
	{
		wprintf(L"%ls:write success:%d\n",file_path.c_str(),ret);
	}
	else
	{
		wprintf(L"%ls:write error!\n",file_path.c_str());
		::fclose(f);
		return -1;
	}
	::fclose(f);
	return 0;
}

// convert wstring to UTF-8 string  
std::string wstring_to_utf8 (const std::wstring& str)  
{  
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;  
    return myconv.to_bytes(str);  
}  

void un_pack(ttstr data_file,ttstr dst_dir)
{
	tTVPXP3Archive* xp3_arc = new tTVPXP3Archive(data_file);
	std::vector<char> buf;
	buf.resize(1024);
	int count = xp3_arc->GetCount();
	wprintf(L"xp3 data file count:%d\n",count);
	int fail_count = 0;
	for(int idx = 1; idx < count; ++idx)
	{
		ttstr filename= xp3_arc->GetName(idx);
		tTVPXP3ArchiveStream* s = (tTVPXP3ArchiveStream*) xp3_arc->CreateStreamByIndex(idx);
		int size = s->GetSize();
		wprintf(L"idx:%d,unpacking... name:%ls,org_size:%d,hash:%0x\n",idx,
				filename.c_str(),size,xp3_arc->GetFileHash(idx));
				// char* buf = new char(s->GetSize()+1);
		if(buf.capacity() < size)
		{
			buf.resize(size+10);
		}
		//text file.
		if(filename.AsStdString().find(L".tjs")!=std::wstring::npos ||
		filename.AsStdString().find(L".ks")!=std::wstring::npos
			)
		{
			wprintf(L"in text file:%ls!!\n",filename.c_str());
			auto ss = TVPCreateTextStreamForReadWithStream(filename,TJS_W(""),s);
			ttstr buffer;
			ss->Read(buffer,0);
			// buffer.ToNarrowStr((char*)buf.data(),buf.capacity());
			std::string new_s = wstring_to_utf8(buffer.AsStdString());
			// wprintf(L"szie:%d,%d\n",buffer.GetLen(),size);
			int r = write_file(dst_dir + TJS_W("/") + xp3_arc->GetName(idx),
				(void*)new_s.c_str(),
				new_s.size(),true);
			if(r<0)
				fail_count++;
			continue;
		}
		
		s->Read(buf.data(),size);
		int r = write_file(dst_dir + TJS_W("/") + xp3_arc->GetName(idx),buf.data(),
			size);
		if(r<0)
			fail_count++;
	}
	wprintf(L"unpack ok,fail_count:%d\n",fail_count);


}

int main()
{
	setlocale(LC_ALL, "");
	Application = new tTVPApplication;
	//Application->PrintConsole("1哈哈哈!\n",7);
	TVPLoadMessage();
	TVPSetXP3ArchiveExtractionFilter(xp3_filter);
	

	try
	{

		// TVPSystemInit();
		TVPSetCurrentDirectory(ExePath());
		un_pack(TJS_W("voice.xp3"),"voice");
		// tTVPXP3Archive* xp3_arc = new tTVPXP3Archive(TJS_W("data.xp3"));
		// for(int idx=0;idx<2;idx++)
		// {
		// 	tTVPXP3ArchiveStream* s = (tTVPXP3ArchiveStream*) xp3_arc->CreateStreamByIndex(idx);
		// 	wprintf(L"name:%ls,org_size:%d,hash:%0x\n",
		// 		xp3_arc->GetName(idx).c_str(),s->GetSize(),xp3_arc->GetFileHash(idx));

		// }


		// TVPInitializeStartupScript();
	}
	catch(eTJSError &e)
	{
		// printf("error:%s\n",e.GetMessage().AsNarrowStdString().c_str());
		Application->PrintConsole(e.GetMessage().c_str(),e.GetMessage().GetLen());
	}


	return 0;
}
