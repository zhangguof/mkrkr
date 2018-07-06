#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
// #include <mach/error.h>
#include <boost/filesystem.hpp>
#include <vector>
#include <codecvt>
#include <locale.h>

#include "tjsCommHead.h"
#include "XP3Archive.h"
#include "TextStream.h"

extern void TVPLoadMessage();
extern void TVPSetXP3ArchiveExtractionFilter(tTVPXP3ArchiveExtractionFilter filter);
extern bool TVPCheckExistentLocalFile(const ttstr &name);

#ifdef WIN32
void _stdcall xp3_filter(tTVPXP3ExtractionFilterInfo* info)
#else
void  xp3_filter(tTVPXP3ExtractionFilterInfo* info)
#endif
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



void get_file_list(ttstr data_file)
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
		wprintf(L"idx:%d,name:%ls,org_size:%d,hash:%0x\n",idx,
				filename.c_str(),size,xp3_arc->GetFileHash(idx));
				// char* buf = new char(s->GetSize()+1);
	}
	delete xp3_arc;
}

// convert wstring to UTF-8 string  
std::string wstring_to_utf8 (const std::wstring& str)  
{  
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;  
    return myconv.to_bytes(str);  
}

int write_file(ttstr file_path, void* buf, int size, bool over_write=false)
{

	if(!over_write && TVPCheckExistentLocalFile(file_path))
	{
		wprintf(L"exist file:%d,skip:%ls\n",over_write,file_path.c_str());
		return 0;
	}

	tTJSBinaryStream* w_stream = TVPCreateStream(file_path,TJS_BS_WRITE);

	if(w_stream==NULL)
	{

		wprintf(L"open stream failed:%ls\n",file_path.c_str());
		return 0;
	}
	int ret = w_stream->Write(buf,size);
	if(ret)
	{
		wprintf(L"%ls:write success:%d\n",file_path.c_str(),ret);
	}
	else
	{
		wprintf(L"%ls:write error!\n",file_path.c_str());
		delete w_stream;
		return 0;
	}
	delete w_stream;
	return ret;
}

const int max_buf_size = 8*1024;

int write_bin_file(ttstr file_path, tTVPXP3ArchiveStream* src_stream,
			   bool over_write=false)
{

	if(!over_write && TVPCheckExistentLocalFile(file_path))
	{
		wprintf(L"exist file:%d,skip:%ls\n",over_write,file_path.c_str());
		return 0;
	}

	tTJSBinaryStream* w_stream = TVPCreateStream(file_path,TJS_BS_WRITE);

	if(w_stream==NULL)
	{

		wprintf(L"open stream failed:%ls\n",file_path.c_str());
		return 0;
	}
	int size = src_stream->GetSize();
	tjs_uint8 buf[max_buf_size];
	int remain = size;
	int read_size = remain > max_buf_size?max_buf_size:remain;
	int write_size = 0;
	while (remain > 0)
	{
		tjs_uint32 has_read = src_stream->Read(buf,read_size);
		if(has_read == 0)
			break;
		int has_write = w_stream->Write(buf,has_read);
		write_size += has_write;
		remain -= has_read;
		read_size = remain > max_buf_size?max_buf_size:remain;
	}
	assert(write_size == size);

	// int ret = w_stream->Write(buf,size);
	if(write_size)
	{
		wprintf(L"%ls:write success:%d\n",file_path.c_str(),write_size);
	}
	else
	{
		wprintf(L"%ls:write error!\n",file_path.c_str());
		delete w_stream;
		return 0;
	}
	delete w_stream;
	return write_size;
}



int unpack_arc_idx(tTVPXP3Archive* xp3_arc,ttstr dst_dir,int idx,bool is_bin = false)
{
	// tjs_uint8 buf[max_buf_size];
	ttstr filename= xp3_arc->GetName(idx);
	ttstr dst_path = dst_dir + L"/" + filename;
	tTVPXP3ArchiveStream* s = (tTVPXP3ArchiveStream*) xp3_arc->CreateStreamByIndex(idx);
	int size = s->GetSize();
	wprintf(L"idx:%d,unpacking... name:%ls,org_size:%d,hash:%0x\n",idx,
				filename.c_str(),size,xp3_arc->GetFileHash(idx));

	//text file.
	if(!is_bin && (filename.AsStdString().find(L".tjs")!=std::wstring::npos ||
				  filename.AsStdString().find(L".ks")!=std::wstring::npos)
		)
	{
		wprintf(L"in text file:%ls!!\n",filename.c_str());
		iTJSTextReadStream* text_stream = NULL;
		try
		{
			text_stream = TVPCreateTextStreamForReadWithStream(filename,TJS_W(""),s);
			// s = NULL;
		}
		catch(eTJSError &e)
		{
			wprintf(L"try to read as Bin file:%ls\n",filename.c_str());
			// s->Seek(0,TJS_BS_SEEK_SET);
			if(s) delete s;
			s = (tTVPXP3ArchiveStream*)xp3_arc->CreateStreamByIndex(idx);
			int r = write_bin_file(dst_path,s);
			// s = NULL;
			delete s;
			return r;
		}
		// auto ss = TVPCreateTextStreamForReadWithStream(filename,TJS_W(""),s);
		ttstr buffer;
		text_stream->Read(buffer,0);
		assert(text_stream);
		delete text_stream

		std::string new_s = wstring_to_utf8(buffer.AsStdString());
		int r = write_file(dst_dir + TJS_W("/") + xp3_arc->GetName(idx),
			(void*)new_s.c_str(),
			new_s.size(),true);
		return r;
	}
	// s->Read(buf,size);
	// int r = write_file(dst_dir + TJS_W("/") + xp3_arc->GetName(idx),
	// 				   buf,size);
	int r = write_bin_file(dst_dir+TJS_W("/")+ xp3_arc->GetName(idx),s);

	return r;
}

void do_unpack(ttstr data_file,ttstr dst_dir)
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
		// ttstr dst_path = dst_dir + L"/" + filename;
		// tTVPXP3ArchiveStream* s = (tTVPXP3ArchiveStream*) xp3_arc->CreateStreamByIndex(idx);
		// int size = s->GetSize();
		int r = unpack_arc_idx(xp3_arc,dst_dir,idx);
		if(r<=0)
			fail_count++;
		// wprintf(L"idx:%d,unpacking... name:%ls,org_size:%d,hash:%0x\n",idx,
		// 		filename.c_str(),size,xp3_arc->GetFileHash(idx));
				// char* buf = new char(s->GetSize()+1);
		// if(buf.capacity() < size)
		// {
		// 	buf.resize(size+10);
		// }
		//text file.
		// if(filename.AsStdString().find(L".tjs")!=std::wstring::npos ||
		//    filename.AsStdString().find(L".ks")!=std::wstring::npos)
		// {
		// 	wprintf(L"in text file:%ls!!\n",filename.c_str());
		// 	iTJSTextReadStream* text_stream = NULL;
		// 	try
		// 	{
		// 		text_stream = TVPCreateTextStreamForReadWithStream(filename,TJS_W(""),s);
		// 	}
		// 	catch(eTJSError &e)
		// 	{
		// 		wprintf(L"try to read as Bin file:%ls\n",filename.c_str());
		// 		s->Seek(0,TJS_BS_SEEK_SET);
		// 		int r = write_bin_file(dst_path,s);
		// 		if(r<0)
		// 			fail_count++;
		// 		continue;

		// 	}
		// 	// auto ss = TVPCreateTextStreamForReadWithStream(filename,TJS_W(""),s);
		// 	ttstr buffer;
		// 	text_stream->Read(buffer,0);
		// 	std::string new_s = wstring_to_utf8(buffer.AsStdString());
		// 	int r = write_file(dst_path,
		// 						(void*)new_s.c_str(),
		// 						new_s.size(),true);
		// 	if(r<0)
		// 		fail_count++;
		// }
		
		// // s->Read(buf.data(),size);
		// int r = write_bin_file(dst_path,s);
		// if(r<0)
		// 	fail_count++;
	}
	wprintf(L"unpack ok,fail_count:%d\n",fail_count);
}


const int max_arg_cnt = 20;
int _argc;
// tjs_char ** _wargv;
ttstr _argv[max_arg_cnt];

int main(int argc,char* argv[])
{
	setlocale(LC_CTYPE, "UTF-8");
	TVPLoadMessage();
	TVPSetXP3ArchiveExtractionFilter(xp3_filter);
	// get_file_list(TJS_W("../repos/data.xp3"));
	try
	{
		ttstr fname = L"../repos/data.xp3";
		ttstr dst_dir = L"../repos/data1";
		_argc = argc;
		for(int i= 0;i<argc;++i)
		{
			_argv[i] = argv[i];
			// TVPAddLog(_argv[i]);
		}
		for(int i=0;i<argc;++i)
		{
			if(_argv[i] == TJS_W("-data") && i+1<argc)
			{
				fname = _argv[i+1];
				continue;
			}
			if(_argv[i] == TJS_W("-out") && i+1<argc)
			{
				dst_dir = _argv[i+1];
				continue;
			}
		}
		wprintf(L"src_data:%ls,dst_dir:%ls\n",fname.c_str(),dst_dir.c_str());
		
		// tTVPXP3Archive* xp3_arc = new tTVPXP3Archive(fname);
		// unpack_arc_idx(xp3_arc,dst_dir,497);
		do_unpack(fname,dst_dir);
	}
	catch(eTJSError &e)
	{
		wprintf(L"error:%ls\n",e.GetMessage().c_str());
		// Application->PrintConsole(e.GetMessage().c_str(),e.GetMessage().GetLen());
	}

	return 0;
}

