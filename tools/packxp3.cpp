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
#include <string>
#include <vector>

#include "tjsCommHead.h"
#include "XP3Archive.h"
#include "TextStream.h"
#include "tjsHashSearch.h"

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

//===============================================
//like ホ ゙---> ボ(\u30db \u3099) ->\u30dc
void fix_jp_path(ttstr& name)
{
	tjs_char* p = name.Independ();
	tjs_char* pbuf = p;
	while(*p)
	{
		if(*p == 0x3099 || *p == 0x309a)
		{
			*(pbuf-1) = *(pbuf-1)+(*p-0x3098);
			++p;
		}
		else
		{
			if(pbuf!=p)
				*pbuf++ = *p++;
			else
			{
				++pbuf;
				++p;
			}
		}
	}
	*pbuf = 0;
	name.FixLen();
}

#define TVP_AUTO_PATH_HASH_SIZE 1024
// std::vector<ttstr> TVPAutoPathList;
tTJSHashCache<ttstr, ttstr> PathCache(TVP_AUTO_PATH_HASH_SIZE);


typedef std::vector<std::pair<ttstr,ttstr> > tFileList;
typedef std::shared_ptr<tFileList> tPFileList ;


class FileBuffer
{
public:
	char* buf;
	// tTJSBinaryStream* stream;
	uint32_t filesize;
	int32_t filehash;
	ttstr name;
	FileBuffer(){
		buf = nullptr;
		// stream = nullptr;
		filesize = 0;
		filehash = 0;
	}
	void open(ttstr filename)
	{
		name =filename;
		auto stream = TVPCreateStream(filename);

		filesize = stream->GetSize();
		buf = new char[filesize+1];
		int r = stream->Read(buf,filesize);
		assert(r == filesize);
		filehash = 0;

		delete stream;
		stream = NULL;
	}
	uint64_t write(tTJSBinaryStream* st)
	{
		if(buf)
		{
			st->WriteBuffer(buf,filesize);
			// assert(r == filesize);
			printf("file write:%ls::%d\n", name.c_str(),filesize);
			return filesize;
		}
		return 0;
	}
	~FileBuffer(){
		if(buf)
		{
			delete[] buf;
			printf("==relase file buf.\n");
		}
		// if(stream)
		// 	delete stream;
	}
};



std::shared_ptr<FileBuffer> read_file(ttstr file)
{
	auto pfile = std::make_shared<FileBuffer>();
	pfile->open(file);
	printf("read file:%ls:size:%d\n",pfile->name.c_str(),pfile->filesize);
	return pfile;

}


void get_file_list(ttstr name,tPFileList lister)
{
	PathCache.Clear();
	lister->clear();

	boost::filesystem::recursive_directory_iterator iEnd;
	boost::filesystem::path base_path(name.c_str());
	if(!boost::filesystem::exists(base_path))
	{
		printf("file:%ls not exists!\n", name.c_str());
		return;
	}


	for(auto p= boost::filesystem::recursive_directory_iterator(name.c_str());
		p!=iEnd;++p)
	{
		auto t = p->status().type();
		if(t == boost::filesystem::regular_file)
		{
			//path relative(const path& p, const path& base
			boost::system::error_code ec;
			auto rel_path = boost::filesystem::relative(p->path(),base_path,ec);
			ttstr file(rel_path.c_str());
			ttstr filepath(p->path().c_str());
			tjs_char *pc = file.Independ();
			while(*pc)
			{
				// make all characters small
				if(*pc >= TJS_W('A') && *pc <= TJS_W('Z'))
					*pc += TJS_W('a') - TJS_W('A');
				pc++;
			}
			fix_jp_path(file);
			if(file == L".ds_store") continue;
			lister->push_back(std::make_pair(filepath,file));
			PathCache.Add(file,file);
			// printf("add file:%ls:namehash:0x%0x\n",file.c_str(),TJS::tTJSHashFunc<ttstr>::Make(file));
			// printf("file:%ls,hash:0x%0x\n",p->path().c_str(),);
			// read_file(p->path().c_str());
			printf("add file:%ls,%ls",filepath.c_str(),file.c_str());

		}
	}

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



//int type write
template<typename I>
uint64_t StreamWrite(tTJSBinaryStream* st,I& val)
{
	st->WriteBuffer((void*)&val,sizeof(I));
	return sizeof(I);
}

struct BaseChunk
{
	char mark[4];
	uint64_t size;
	virtual uint64_t write(tTJSBinaryStream* st)
	{
		st->WriteBuffer(mark,4);
		// st->WriteBuffer((void*)&size,sizeof(uint64_t));
		StreamWrite(st,size);
		return sizeof(size) + 4;
	}
	tjs_uint64 get_total_size()
	{
		return size + 4 + 8;
	}
};
struct InfoChunk: public BaseChunk
{
	int32_t flags;
	int64_t OrgSize;
	int64_t ArcSize;
	// int16_t name_len;
	ttstr name;
	InfoChunk(int32_t f,int64_t org_size,
		int64_t arc_size,ttstr _name)
	{
		flags = f;
		OrgSize = org_size;
		ArcSize = arc_size;
		name = _name;
		char _mark[4] = {'i','n','f','o'};
		memcpy(mark,_mark,4);
		size = sizeof(int32_t) + sizeof(int64_t)*2 +
			   sizeof(int16_t) + name.length() * 2; //using utf16 as name encode.

	}
	uint64_t write(tTJSBinaryStream* st)
	{
		uint64_t w_size = BaseChunk::write(st);
		// st->WriteBuffer((void*)&flags,sizeof(flags));
		w_size += StreamWrite(st,flags);
		w_size += StreamWrite(st,OrgSize);
		w_size += StreamWrite(st,ArcSize);
		uint16_t name_len = name.length();
		w_size += StreamWrite(st,name_len);

		uint16_t tmp[name_len];
		const tjs_char* p = name.c_str();
		for(int i=0;i<name_len;++i)
		{
			tmp[i] = p[i];
		}
		st->WriteBuffer((void*)tmp,name_len * sizeof(uint16_t));
		w_size += name_len * sizeof(uint16_t);
		assert(w_size == get_total_size());
		return w_size;
	}

};

struct SegmChunk:public BaseChunk
{
	struct segm_item //sizeof(segm_item == 28)
	{
		int32_t flags;
		int64_t start;
		int64_t OrgSize;
		int64_t ArcSize;
		uint64_t write(tTJSBinaryStream *st)
		{
			StreamWrite(st,flags);
			StreamWrite(st,start);
			StreamWrite(st,OrgSize);
			StreamWrite(st,ArcSize);
			return sizeof(int32_t) + sizeof(uint64_t)*3;
		}
	};
	// int segm_cnt;
	std::vector<segm_item> v;
	SegmChunk()
	{
		// assert(cnt>0);
		// segm_cnt = 0;
		size = 0;
		char _mark[4] = {'s','e','g','m'};
		memcpy(mark,_mark,4);
		// size = sizeof(segm_item) * cnt;
	}
	void add_segm(bool IsCompressed,int64_t start,int64_t OrgSize,int64_t ArcSize)
	{
		segm_item item;
		item.flags = IsCompressed?TVP_XP3_SEGM_ENCODE_ZLIB:TVP_XP3_SEGM_ENCODE_RAW;
		item.start = start;
		item.OrgSize = OrgSize;
		item.ArcSize = ArcSize;
		v.push_back(item);
		// ++cnt;
		// size += sizeof(segm_item);
		size += 28;
	}
	uint64_t write(tTJSBinaryStream* st)
	{
		uint64_t w_size = BaseChunk::write(st);
		for(auto i:v)
		{
			w_size += i.write(st);
		}
		assert(w_size == 12 + v.size() * 28);
		printf("segms write:%d,size:%lu\n",v.size(),w_size);
	}


};
struct AdlrChunk:public BaseChunk
{
	int32_t FileHash;
	AdlrChunk(int32_t file_hash)
	{
		FileHash = file_hash;
		char _mark[] = {'a','d','l','r'};
		memcpy(mark,_mark,4);
		size = sizeof(int32_t);
	}
	uint64_t write(tTJSBinaryStream* st)
	{
		uint64_t w_size = BaseChunk::write(st);
		w_size += StreamWrite(st,FileHash);
		assert(w_size == 12 + 4);
		return w_size;
	}
};

struct FileChunk:public BaseChunk
{
	InfoChunk* info;
	SegmChunk* segms;
	AdlrChunk* adlr;
	FileChunk(ttstr filepath,ttstr filename,tTJSBinaryStream* st)
	{
		// tTJSBinaryStream* read_s = TVPCreateStream(filepath);
		FileBuffer fb;
		fb.open(filepath);
		info = new InfoChunk(0,fb.filesize,fb.filesize,filename);
		
		segms = new SegmChunk();
		//write file data in archive.
		auto cur_pos = st->GetPosition();
		auto w_size = fb.write(st);
		segms->add_segm(false,cur_pos,w_size,w_size);
		adlr = new AdlrChunk(fb.filehash);

		char _mark[] = {'F','i','l','e'};
		memcpy(mark,_mark,4);
		size = info->get_total_size() + segms->get_total_size() + 
			   adlr->get_total_size();
		printf("FileChunk:%d,info_size:%d,segms_size:%d,adlr_size:%d\n",
				size,info->get_total_size(),segms->get_total_size(),
				adlr->get_total_size());

	}
	uint64_t write(tTJSBinaryStream* st)
	{
		uint64_t w_size = BaseChunk::write(st);
		w_size += info->write(st);
		w_size += segms->write(st);
		w_size += adlr->write(st);
		return w_size;
	}
	~FileChunk()
	{
		delete info;
		delete segms;
		delete adlr;
	}

};


struct Indices
{
	uint64_t index_ofs_pos;
	uint64_t index_ofs;
	uint8_t index_flag;
	uint64_t index_size;
	uint64_t next_ofs_pos;
	// Indices* next;
	// Indices* prev;
	std::shared_ptr<Indices> next;
	// std::shared_ptr<Indices> prev;
	tPFileList pfiles;
	std::vector<std::shared_ptr<FileChunk> > pchunks;

	// std::vector<FileChunk> filechunks;
	Indices(uint64_t index_pos,uint8_t flag,tPFileList files)
	{
		pchunks.clear();
		index_ofs_pos = index_pos; //save cur indices's index_ofs pos.
		index_size = 0;
		index_flag = flag;
		next_ofs_pos = 0;
		next = nullptr;
		pfiles = files;
	}
	~Indices()
	{
		if(next) next = nullptr;
		if(pfiles) pfiles = nullptr;
		pchunks.clear();

		printf("release Indices!\n");
	}
	void create_file_chunks(tTJSBinaryStream* st)
	{
		if(pfiles)
		{
			auto files = *pfiles;
			for(auto i:files)
			{
				//will do write file buffer;
				auto file_chunk = std::make_shared<FileChunk>(i.first,i.second,st);
				pchunks.push_back(file_chunk);
				index_size += file_chunk->get_total_size();
			}
		}
	}
	// uint64_t write_data(tTJSBinaryStream* st)
	// {
	// 	return 0;
	// }
	uint64_t write(tTJSBinaryStream* st)
	{
		// write_data(st);//wirte files content
		create_file_chunks(st);
		//has write all file data;
		uint64_t cur_pos = st->GetPosition();
		//try rewrite index_ofs
		st->Seek(index_ofs_pos,TJS_BS_SEEK_SET);
		StreamWrite(st, cur_pos);
		st->Seek(cur_pos,TJS_BS_SEEK_SET);
		StreamWrite(st,index_flag);
		StreamWrite(st,index_size);
		for(auto i:pchunks)
		{
			i->write(st);
			printf("writing file chunk\n");
		}
		next_ofs_pos = st->GetPosition();
		uint64_t next_index = 0; //defual will write by next indice
		StreamWrite(st,next_index);//next_index_ofs;
		printf("indices write success!:next:%u\n",next_ofs_pos);

	}
};

struct IndicesList
{
	std::shared_ptr<Indices> head;
	std::shared_ptr<Indices> tail;
	IndicesList(uint64_t start_offset_pos)
	{
		//first empty indices
		head = std::make_shared<Indices>(start_offset_pos,
					TVP_XP3_INDEX_ENCODE_RAW|TVP_XP3_INDEX_CONTINUE,
					nullptr);
		tail = head;
	}

	void addIndices(uint8_t index_flag,tPFileList pfiles)
	{
		//ensure has tail has write!
		assert(tail->next_ofs_pos!=0);
		auto next = std::make_shared<Indices>(tail->next_ofs_pos,index_flag,pfiles);
		tail->next = next;
		tail = next;
	}
	std::shared_ptr<Indices> get_tail()
	{
		return tail;
	}
	~IndicesList()
	{
		printf("release Indices list!\n");
		if(tail) tail = nullptr;
		if(head) head = nullptr;

	}
};


class WriteableArchive
{
public:
	//one FileChunk
	// struct tArchiveItem
	// {
	// 	ttstr Name;
	// 	tjs_uint32 FileHash;
	// 	tjs_uint64 OrgSize; // original ( uncompressed ) size
	// 	tjs_uint64 ArcSize; // in-archive size
	// 	// std::vector<tTVPXP3ArchiveSegment> Segments;
	// 	bool operator < (const tArchiveItem &rhs) const
	// 	{
	// 		return this->Name < rhs.Name;
	// 	}
	// };
	// struct Index
	// {
	// 	tjs_uint8 index_flag;
	// 	tjs_uint64 index_size;
	// 	char* buf;
	// 	void write(tTJSBinaryStream* st)
	// 	{

	// 	}
	// };
	// std::vector<tArchiveItem> ItemVector;
public:
	WriteableArchive(const ttstr &dst_name, const ttstr &src_path);
	// void add_item(ttstr name, tjs_uint32 FileHash,
	// tjs_uint64 OrgSize,tjs_uint64 ArcSize);

static void writeArchMark(tTJSBinaryStream* st,tjs_uint64& offset);

	// void writeIdxData();
};

// void WriteableArchive::add_item(ttstr name, tjs_uint32 FileHash,
// 		 tjs_uint64 OrgSize,tjs_uint64 ArcSize)
// {
// 		tArchiveItem item;
// 		item.Name = name;
// 		item.FileHash = FileHash;
// 		item.OrgSize = OrgSize;
// 		item.ArcSize = ArcSize;
// 		ItemVector.push_back(item);
// }

void WriteableArchive::writeArchMark(tTJSBinaryStream* st,tjs_uint64& offset)
{
	static tjs_uint8 XP3Mark1[] =
	{ 0x58/*'X'*/, 0x50/*'P'*/, 0x33/*'3'*/, 0x0d/*'\r'*/,
	  0x0a/*'\n'*/, 0x20/*' '*/, 0x0a/*'\n'*/, 0x1a/*EOF*/,
	  0xff /* sentinel */ };
	static tjs_uint8 XP3Mark2[] =
	{ 0x8b, 0x67, 0x01, 0xff/* sentinel */ };
	static tjs_uint8 XP3Mark[11+1];

	static bool DoInit = true;
	if(DoInit)
	{
		// the XP3 header above is splitted into two part; to avoid
		// mis-finding of the header in the program's initialized data area.
		DoInit = false;
		memcpy(XP3Mark, XP3Mark1, 8);
		memcpy(XP3Mark + 8, XP3Mark2, 3);
		// here joins it.
	}
	st->WriteBuffer(XP3Mark,11);
	offset+=11;
}

WriteableArchive::WriteableArchive(const ttstr &dst_name, const ttstr &src_path)
{
	// static tjs_uint8 cn_File[] =
	// 	{ 0x46/*'F'*/, 0x69/*'i'*/, 0x6c/*'l'*/, 0x65/*'e'*/ };
	// static tjs_uint8 cn_info[] =
	// 	{ 0x69/*'i'*/, 0x6e/*'n'*/, 0x66/*'f'*/, 0x6f/*'o'*/ };
	// static tjs_uint8 cn_segm[] =
	// 	{ 0x73/*'s'*/, 0x65/*'e'*/, 0x67/*'g'*/, 0x6d/*'m'*/ };
	// static tjs_uint8 cn_adlr[] =
	// 	{ 0x61/*'a'*/, 0x64/*'d'*/, 0x6c/*'l'*/, 0x72/*'r'*/ };
	tjs_uint64 offset;

	auto f_list = std::make_shared<tFileList>();
	// ItemVector.clear();
	get_file_list(src_path,f_list);
	auto dst_s = TVPCreateStream(dst_name,TJS_BS_WRITE);
	offset = 0;

	// for(auto i:v)
	// {
	// 	add_item(i,0,0,0);
	// }
	writeArchMark(dst_s,offset);
	uint64_t indices_start = 0;
	StreamWrite(dst_s,indices_start);//first indices offest
	auto indices_list = std::make_shared<IndicesList>(offset);
	indices_list->tail->write(dst_s);
	//void addIndices(uint8_t index_flag,tPFileList pfiles)
	indices_list->addIndices(TVP_XP3_INDEX_ENCODE_RAW,f_list);
	indices_list->tail->write(dst_s);

}

void do_pack(ttstr data_file,ttstr src_dir)
{
	WriteableArchive* arc = new WriteableArchive(data_file, src_dir);

	delete arc;
	printf("finish pack!\n");
	// std::vector<ttstr> v;
	// get_file_list(src_dir,v);
	// auto dst_s = TVPCreateStream(data_file,TJS_BS_WRITE);

	// tTVPXP3Archive* xp3_arc = new tTVPXP3Archive(data_file);
	// std::vector<char> buf;
	// buf.resize(1024);
	// int count = xp3_arc->GetCount();
	// wprintf(L"xp3 data file count:%d\n",count);
	// int fail_count = 0;
	// for(int idx = 1; idx < count; ++idx)
	// {
	// 	ttstr filename= xp3_arc->GetName(idx);
	// 	int r = unpack_arc_idx(xp3_arc,dst_dir,idx);
	// 	if(r<=0)
	// 		fail_count++;
	// }
	// wprintf(L"unpack ok,fail_count:%d\n",fail_count);
}


const int max_arg_cnt = 20;
int _argc;
// tjs_char ** _wargv;
ttstr _argv[max_arg_cnt];

int main(int argc,char* argv[])
{
	setlocale(LC_CTYPE, "UTF-8");
	TVPLoadMessage();
	try
	{
		ttstr fname = L"data.xp3";
		ttstr src_dir = L"testdata";
		_argc = argc;
		for(int i= 0;i<argc;++i)
		{
			_argv[i] = argv[i];
			// TVPAddLog(_argv[i]);
		}
		for(int i=0;i<argc;++i)
		{
			if(_argv[i] == TJS_W("-out") && i+1<argc)
			{
				fname = _argv[i+1];
				continue;
			}
			if(_argv[i] == TJS_W("-src") && i+1<argc)
			{
				src_dir = _argv[i+1];
				continue;
			}
		}
		wprintf(L"dst_datafile:%ls,src_dir:%ls\n",fname.c_str(),src_dir.c_str());
		
		do_pack(fname,src_dir);
	}
	catch(eTJSError &e)
	{
		wprintf(L"error:%ls\n",e.GetMessage().c_str());
		// Application->PrintConsole(e.GetMessage().c_str(),e.GetMessage().GetLen());
	}

	return 0;
}

