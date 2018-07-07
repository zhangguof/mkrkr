#include <windows.h>
#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#include <boost/filesystem.hpp>
#include <vector>
#include <codecvt>

#include <locale.h> 

#include "tp_stub.h"
#include <wchar.h>
#include "XP3Archive.h"
#include <map>
#include <memory>
#include "load_infos.h"

typedef std::map<int,ttstr> tFileList;

//class tTVPTextReadStream;

//extern iTJSTextReadStream * TVPCreateTextStreamForReadWithStream(const ttstr & name,
//	const ttstr & modestr, tTJSBinaryStream* s);
//
//
//void xp3_filter(tTVPXP3ExtractionFilterInfo* info)
//{
//	// assert(info->SizeOfSelf == 0x18);
//	tjs_uint8 * buff = (tjs_uint8*) (info->Buffer);
//	tjs_uint32 file_hash = info->FileHash;
//	tjs_uint8 h = file_hash & 0xff;
//	for(tjs_uint i=0;i< info->BufferSize;++i)
//	{
//		tjs_uint8 a = buff[i];
//		a = a ^ (h + 1);
//		a = ~a;
//		buff[i] = a;
//	}
//}

tTJSBinaryStream * TVPCreateStream(const ttstr & name, tjs_uint32 flags)
{
	IStream* _is = TVPCreateIStream(name,flags);
	//tTJSBinaryStream* s = TVPCreateBinaryStreamAdapter(_is);
	tTJSBinaryStream* s = TVPCreateBinaryStreamAdapter_ex(_is);
	return s;
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
int write_bin_file2(ttstr file_path, tTJSBinaryStream* src_stream)
{
	boost::filesystem::path p(file_path.c_str());
	auto pp = p.parent_path();

	if(!boost::filesystem::exists(pp))
	{
		if(!(boost::filesystem::create_directories(pp)))
		{
			wprintf(L"create dirs error:%ls\n",pp.c_str());
			return -1;
		}
	}
	FILE* f = NULL;
	auto err = ::_wfopen_s(&f,p.c_str(),L"wb");
	if(f==NULL)
	{
		wprintf(L"open file:%ls error:%d",p.c_str(),err);
		return -1;
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
		int has_write = ::fwrite(buf,sizeof(uint8_t),has_read,f);
		write_size += has_write;
		remain -= has_read;
		read_size = remain > max_buf_size?max_buf_size:remain;
	}
	assert(write_size == size);

	if(write_size)
	{
		wprintf(L"%ls:write success:%d\n",file_path.c_str(),write_size);
	}
	else
	{
		wprintf(L"%ls:write error!\n",file_path.c_str());
		::fclose(f);
		return 0;
	}
	::fclose(f);
	return write_size;
}

int write_bin_file(ttstr file_path, tTJSBinaryStream* src_stream,
				   bool over_write=false)
{

	if(!over_write && TVPCheckExistentLocalFile(file_path))
	{
		wprintf(L"exist file:%d,skip:%ls\n",over_write,file_path.c_str());
		return 0;
	}
	ttstr ppath = TVPExtractStoragePath(file_path);
	wprintf(L"extract path:%ls\n",ppath.c_str());
	if(!TVPCheckExistentLocalFolder(ppath))
	{
		TVPCreateFolders(ppath);
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
int unpack_one_file(ttstr data_file,ttstr filename,ttstr dst_dir,bool over_write = false)
{
	ttstr src_path = data_file + L">" + filename;
	ttstr dst_path = dst_dir + "/" + filename;
	boost::filesystem::path p(dst_path.c_str());

	wprintf(L"unpack one file:%ls\n",src_path.c_str());
	if(!over_write && boost::filesystem::exists(p))
	{
		wprintf(L"exist file:%ls,skip it!\n",p.c_str());
		return 0;
	}
	tTJSBinaryStream* s = TVPCreateStream(src_path,TJS_BS_READ);
	int ret = write_bin_file2(dst_path,s);
	return ret;
}



int unpack_arc_idx(tTVPXP3Archive* xp3_arc,ttstr dst_dir,int idx,bool over_write = false)
{
	ttstr data_file = xp3_arc->GetName();
	ttstr filename= xp3_arc->GetName(idx);
	ttstr dst_path = dst_dir + L"/" + filename;
	ttstr src_path = data_file + L">" + filename;
	wprintf(L"unpack one file:%d:%ls\n",idx,src_path.c_str());

	boost::filesystem::path p(dst_path.c_str());

	if(!over_write && boost::filesystem::exists(p))
	{
		wprintf(L"exist file:%ls,skip it!\n",p.c_str());
		return 0;
	}
	tTJSBinaryStream* s = TVPCreateStream(src_path,TJS_BS_READ);
	int ret = write_bin_file2(dst_path,s);

	return ret;
}



void get_file_list(ttstr data_file, tFileList& file_list)
{
	file_list.clear();
	tTVPXP3Archive* xp3_arc = new tTVPXP3Archive(data_file);
	int count = xp3_arc->GetCount();
	wprintf(L"xp3 data file count:%d\n",count);
	int fail_count = 0;
	for(int idx = 0; idx < count; ++idx)
	{
		ttstr filename= xp3_arc->GetName(idx);
		tTVPXP3ArchiveStream* s = (tTVPXP3ArchiveStream*) xp3_arc->CreateStreamByIndex(idx);
		int size = s->GetSize();
		wprintf(L"idx:%d,name:%ls,org_size:%d,hash:%0x\n",idx,
			filename.c_str(),size,xp3_arc->GetFileHash(idx));
		file_list[idx] = filename;
		// char* buf = new char(s->GetSize()+1);
	}
	delete xp3_arc;
}

int unpack_data_file(ttstr data_file,ttstr dst_dir,int start_idx=1)
{
	wprintf(L"unpacking:%ls\n",data_file.c_str());
	int file_count = 0;
	int fail_count = 0;
	//tFileList file_list;
	//get_file_list(file_path,file_list);

	tTVPXP3Archive* xp3_arc = new tTVPXP3Archive(data_file);
	int count = xp3_arc->GetCount();
	for(int i=start_idx;i<count;++i)
	{
		int r = unpack_arc_idx(xp3_arc,dst_dir,i);
		file_count++;
		if(r<=0)
		{
			fail_count++;
		}
	}
	wprintf(L"===unpack:%ls end:total:%d,fail:%d\n",
			data_file.c_str(),file_count,fail_count);

	delete xp3_arc;

	return file_count;
}


int unpack_main()
{
	ttstr exe_path = TVPGetAppPath();
	//ttstr data_file = L"dtcn.xp3";
	ttstr base_dst_path = L"data1";

	wprintf(L"exe_path:%ls\n",exe_path.c_str());

	auto pinfos = load_infos(L"data_infos.json");
	wprintf(L"data infos =====:\n");
	pinfos->dump();

	int data_count = pinfos->get_count();
	for(int i=0;i<data_count;++i)
	{
		auto p = pinfos->get(i);
		ttstr data_file = p->name.c_str();
		int idx = p->idx;
		ttstr dst_path = base_dst_path + L"/" + p->out_path.c_str();

		ttstr place(TVPGetPlacedPath(exe_path+data_file));
		if(!place.IsEmpty())
		{
			unpack_data_file(place,dst_path,idx);
		}
		else
		{
			ttstr msg = L"can't open xp3 data file:";
			msg = msg + exe_path + data_file;
			printf("%ls\n",msg.c_str());
			
			MessageBox(NULL,msg.c_str(),L"test",MB_OK);
		}


	}


	
	
	
	

	return 0;
}
