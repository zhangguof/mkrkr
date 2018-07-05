#include <stdio.h>
#include <boost/filesystem.hpp>
#include "tjsCommHead.h"


//---------------------------------------------------------------------------
// TVPLocalExtrectFilePath
//---------------------------------------------------------------------------
ttstr TVPLocalExtractFilePath(const ttstr & name)
{
	// this extracts given name's path under local filename rule
	const tjs_char *p = name.c_str();
	tjs_int i = name.GetLen() -1;
	for(; i >= 0; i--)
	{
		if(p[i] == TJS_W(':') || p[i] == TJS_W('/') ||
			p[i] == TJS_W('\\'))
			break;
	}
	return ttstr(p, i + 1);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// TVPCheckExistantLocalFile
//---------------------------------------------------------------------------
bool TVPCheckExistentLocalFile(const ttstr &name)
{
	boost::filesystem::path p(name.c_str());
	return boost::filesystem::exists(p);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPCheckExistantLocalFolder
//---------------------------------------------------------------------------
bool TVPCheckExistentLocalFolder(const ttstr &name)
{
	return TVPCheckExistentLocalFile(name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPCreateFolders
//---------------------------------------------------------------------------
static bool _TVPCreateFolders(const ttstr &folder)
{
	// create directories along with "folder"
	if(folder.IsEmpty()) return true;

	if(TVPCheckExistentLocalFolder(folder))
		return true; // already created

	const tjs_char *p = folder.c_str();
	tjs_int i = folder.GetLen() - 1;

	if(p[i] == TJS_W(':')) return true;

	while(i >= 0 && (p[i] == TJS_W('/') || p[i] == TJS_W('\\'))) i--;

	if(i >= 0 && p[i] == TJS_W(':')) return true;

	for(; i >= 0; i--)
	{
		if(p[i] == TJS_W(':') || p[i] == TJS_W('/') ||
			p[i] == TJS_W('\\'))
			break;
	}

	ttstr parent(p, i + 1);

	if(!_TVPCreateFolders(parent)) return false;

	// BOOL res = ::CreateDirectory(folder.c_str(), NULL);
	boost::filesystem::path _p(folder.c_str());
	bool r = boost::filesystem::create_directory(_p);
	return r;
}

bool TVPCreateFolders(const ttstr &folder)
{
	if(folder.IsEmpty()) return true;

	const tjs_char *p = folder.c_str();
	tjs_int i = folder.GetLen() - 1;

	if(p[i] == TJS_W(':')) return true;

	if(p[i] == TJS_W('/') || p[i] == TJS_W('\\')) i--;

	return _TVPCreateFolders(ttstr(p, i+1));
}
//-

//---------------------------------------------------------------------------
// tTVPLocalFileStream
//---------------------------------------------------------------------------
class tTVPLocalFileStream : public tTJSBinaryStream
{
private:

	typedef FILE* FILEHANDLE;
	FILE* Handle;
	boost::filesystem::path path;

public:
	tTVPLocalFileStream(const ttstr & localname,
		tjs_uint32 flag);
	~tTVPLocalFileStream();

	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence);

	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size);
	tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size);

	void TJS_INTF_METHOD SetEndOfStorage();

	tjs_uint64 TJS_INTF_METHOD GetSize();

	FILEHANDLE GetHandle() const { return Handle; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTVPLocalFileStream
//---------------------------------------------------------------------------
tTVPLocalFileStream::tTVPLocalFileStream(const ttstr &localname, 
						tjs_uint32 flag)
{
	tjs_uint32 access = flag & TJS_BS_ACCESS_MASK;
	const char* mode; 
	switch(access)
	{
	case TJS_BS_READ:
		mode = "rb";
		break;
	case TJS_BS_WRITE:
		mode = "wb";
		break;
	case TJS_BS_APPEND:
		mode = "ab+";
		break;
	case TJS_BS_UPDATE:
		mode = "r+";
		break;
	}
	Handle = ::fopen(localname.AsNarrowStdString().c_str(),mode);
	if(Handle == NULL)
	{
		if(access == TJS_BS_WRITE)
		{
			if(TVPCreateFolders(TVPLocalExtractFilePath(localname)))
			{
				Handle = ::fopen(localname.AsNarrowStdString().c_str(),mode);

			}
		}
	}
	assert(Handle!=NULL);
	path = boost::filesystem::path(localname.c_str());

}
//---------------------------------------------------------------------------
tTVPLocalFileStream::~tTVPLocalFileStream()
{

	if(Handle)
		::fclose(Handle);
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPLocalFileStream::Seek(tjs_int64 offset, tjs_int whence)
{
	int origin;
	switch(whence)
	{
	case TJS_BS_SEEK_SET:
	origin = SEEK_SET;
	break;
	case TJS_BS_SEEK_CUR:
	origin = SEEK_CUR;
	break;
	case TJS_BS_SEEK_END:
	origin = SEEK_END;
	break;
	default:
	origin = SEEK_SET;
	break; // may be enough
	}
	int r = ::fseek(Handle,(long)offset,origin);
	if(r!=0)
		return TJS_UI64_VAL(0xffffffffffffffff);

	tjs_uint64 ret = 0;
	// *(tjs_uint32*)&ret = (tjs_uint32)offset;
	ret = ::ftell(Handle);
	if(ret == (tjs_uint64)-1)
	{
		return TJS_UI64_VAL(0xffffffffffffffff);
	}
	return ret;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPLocalFileStream::Read(void *buffer, tjs_uint read_size)
{
	int ret = ::fread(buffer,sizeof(char),read_size, Handle);
	return ret;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPLocalFileStream::Write(const void *buffer, tjs_uint write_size)
{
	int ret = ::fwrite(buffer,sizeof(char),write_size, Handle);
	return ret;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPLocalFileStream::SetEndOfStorage()
{
	long cur = ftell(Handle);
	boost::filesystem::resize_file(path.c_str(),cur+1);
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPLocalFileStream::GetSize()
{
	tjs_uint64 ret;
	ret = (tjs_uint64) boost::filesystem::file_size(path);
	return ret;
}
//---------------------------------------------------------------------------

tTJSBinaryStream* open_local_stream(const ttstr& name, tjs_uint32 flag)
{
	tTJSBinaryStream* stream = new tTVPLocalFileStream(name,flag);
	return stream;
}
tTJSBinaryStream * TVPCreateStream(const ttstr & name, tjs_uint32 flags)
{
	return open_local_stream(name,flags);
}

//


