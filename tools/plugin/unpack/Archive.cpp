
#include "Archive.hpp"

//error

ERROR_MSG(TVPCannotUnbindXP3EXE);
ERROR_MSG(TVPCannotFindXP3Mark);
ERROR_MSG(TVPInfoTryingToReadXp3VirtualFileSystemInformationFrom);
ERROR_MSG(TVPReadError);
ERROR_MSG(TVPUncompressionFailed);
ERROR_MSG(TVPSpecifiedStorageHadBeenProtected);
ERROR_MSG(TVPInfoFailed);
ERROR_MSG(TVPInfoDoneWithContains);
ERROR_MSG(_TVPXP3ArchiveExtractionFilter);
ERROR_MSG(TVPXP3ArchiveExtractionFilte);
ERROR_MSG(TJSWriteError);
ERROR_MSG(TVPSeekError);
ERROR_MSG(TVPTruncateError);
ERROR_MSG(TVPStorageInArchiveNotFound);
ERROR_MSG(TJSSeekError);
ERROR_MSG(TJSReadError);

const tjs_char* TJSNonamedException = TJS_W("No-named exception");

void TJS_eTJSError(const ttstr & msg) { throw eTJSError(msg); }

//---------------------------------------------------------------------------
// tTJSBinaryStream
//---------------------------------------------------------------------------

void TJS_INTF_METHOD tTJSBinaryStream::SetEndOfStorage()
{
	TJS_eTJSError(TJSWriteError);
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTJSBinaryStream::GetSize()
{
	tjs_uint64 orgpos = GetPosition();
	tjs_uint64 size = Seek(0, TJS_BS_SEEK_END);
	Seek(orgpos, SEEK_SET);
	return size;
}
//---------------------------------------------------------------------------
tjs_uint64 tTJSBinaryStream::GetPosition()
{
	return Seek(0, SEEK_CUR);
}
//---------------------------------------------------------------------------
void tTJSBinaryStream::SetPosition(tjs_uint64 pos)
{
	if(pos != Seek(pos, TJS_BS_SEEK_SET))
		TJS_eTJSError(TJSSeekError);
}
//---------------------------------------------------------------------------
void tTJSBinaryStream::ReadBuffer(void *buffer, tjs_uint read_size)
{
	if(Read(buffer, read_size) != read_size)
		TJS_eTJSError(TJSReadError);
}
//---------------------------------------------------------------------------
void tTJSBinaryStream::WriteBuffer(const void *buffer, tjs_uint write_size)
{
	if(Write(buffer, write_size) != write_size)
		TJS_eTJSError(TJSWriteError);
}
//---------------------------------------------------------------------------
tjs_uint64 tTJSBinaryStream::ReadI64LE()
{
#if TJS_HOST_IS_BIG_ENDIAN
	tjs_uint8 buffer[8];
	ReadBuffer(buffer, 8);
	tjs_uint64 ret = 0;
	for(tjs_int i=0; i<8; i++)
		ret += (tjs_uint64)buffer[i]<<(i*8);
	return ret;
#else
	tjs_uint64 temp;
	ReadBuffer(&temp, 8);
	return temp;
#endif
}
//---------------------------------------------------------------------------
tjs_uint32 tTJSBinaryStream::ReadI32LE()
{
#if TJS_HOST_IS_BIG_ENDIAN
	tjs_uint8 buffer[4];
	ReadBuffer(buffer, 4);
	tjs_uint32 ret = 0;
	for(tjs_int i=0; i<4; i++)
		ret += (tjs_uint32)buffer[i]<<(i*8);
	return ret;
#else
	tjs_uint32 temp;
	ReadBuffer(&temp, 4);
	return temp;
#endif
}
//---------------------------------------------------------------------------
tjs_uint16 tTJSBinaryStream::ReadI16LE()
{
#if TJS_HOST_IS_BIG_ENDIAN
	tjs_uint8 buffer[2];
	ReadBuffer(buffer, 2);
	tjs_uint16 ret = 0;
	for(tjs_int i=0; i<2; i++)
		ret += (tjs_uint16)buffer[i]<<(i*8);
	return ret;
#else
	tjs_uint16 temp;
	ReadBuffer(&temp, 2);
	return temp;
#endif
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTVPBinaryStreamAdapter
//---------------------------------------------------------------------------
tTVPBinaryStreamAdapter::tTVPBinaryStreamAdapter(IStream *ref)
{
	Stream = ref;
	Stream->AddRef();
}
//---------------------------------------------------------------------------
tTVPBinaryStreamAdapter::~tTVPBinaryStreamAdapter()
{
	Stream->Release();
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPBinaryStreamAdapter::Seek(tjs_int64 offset, tjs_int whence)
{
	DWORD origin;

	switch(whence)
	{
	case TJS_BS_SEEK_SET:			origin = STREAM_SEEK_SET;		break;
	case TJS_BS_SEEK_CUR:			origin = STREAM_SEEK_CUR;		break;
	case TJS_BS_SEEK_END:			origin = STREAM_SEEK_END;		break;
	default:						origin = STREAM_SEEK_SET;		break;
	}

	LARGE_INTEGER ofs;
	ULARGE_INTEGER newpos;

	ofs.QuadPart = 0;
	HRESULT hr = Stream->Seek(ofs, STREAM_SEEK_CUR, &newpos);
	bool orgpossaved;
	LARGE_INTEGER orgpos;
	if(FAILED(hr))
	{
		orgpossaved = false;
	}
	else
	{
		orgpossaved = true;
		*(LARGE_INTEGER*)&orgpos = *(LARGE_INTEGER*)&newpos;
	}

	ofs.QuadPart = offset;

	hr = Stream->Seek(ofs, origin, &newpos);
	if(FAILED(hr))
	{
		if(orgpossaved)
		{
			Stream->Seek(orgpos, STREAM_SEEK_SET, &newpos);
		}
		else
		{
			TVPThrowExceptionMessage(TVPSeekError);
		}
	}

	return newpos.QuadPart;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPBinaryStreamAdapter::Read(void *buffer, tjs_uint read_size)
{
	ULONG cb = read_size;
	ULONG read;
	HRESULT hr = Stream->Read(buffer, cb, &read);
	if(FAILED(hr)) read = 0;
	return read;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPBinaryStreamAdapter::Write(const void *buffer, tjs_uint write_size)
{
	ULONG cb = write_size;
	ULONG written;
	HRESULT hr = Stream->Write(buffer, cb, &written);
	if(FAILED(hr)) written = 0;
	return written;
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPBinaryStreamAdapter::GetSize()
{
	HRESULT hr;
	STATSTG stg;

	hr = Stream->Stat(&stg, STATFLAG_NONAME);
	if(FAILED(hr))
	{
		return inherited::GetSize(); // use default routine
	}

	return stg.cbSize.QuadPart;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBinaryStreamAdapter::SetEndOfStorage()
{
	ULARGE_INTEGER pos;
	pos.QuadPart = GetPosition();
	HRESULT hr = Stream->SetSize(pos);
	if(FAILED(hr)) TVPThrowExceptionMessage(TVPTruncateError);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPCreateBinaryStreamAdapter
//---------------------------------------------------------------------------
tTJSBinaryStream * TVPCreateBinaryStreamAdapter_ex(IStream *refstream)
{
	return new  tTVPBinaryStreamAdapter(refstream);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTVPArchive
//---------------------------------------------------------------------------
void tTVPArchive::NormalizeInArchiveStorageName(ttstr & name)
{
	// normalization of in-archive storage name does :
	if(name.IsEmpty()) return;

	// make all characters small
	// change '\\' to '/'
	tjs_char *ptr = name.Independ();
	while(*ptr)
	{
		if(*ptr >= TJS_W('A') && *ptr <= TJS_W('Z'))
			*ptr += TJS_W('a') - TJS_W('A');
		else if(*ptr == TJS_W('\\'))
			*ptr = TJS_W('/');
		ptr++;
	}

	// eliminate duplicated slashes
	ptr = name.Independ();
	tjs_char *org_ptr = ptr;
	tjs_char *dest = ptr;
	while(*ptr)
	{
		if(*ptr != TJS_W('/'))
		{
			*dest = *ptr;
			ptr ++;
			dest ++;
		}
		else
		{
			if(ptr != org_ptr)
			{
				*dest = *ptr;
				ptr ++;
				dest ++;
			}
			while(*ptr == TJS_W('/')) ptr++;
		}
	}
	*dest = 0;

	name.FixLen();
}
//---------------------------------------------------------------------------
void tTVPArchive::AddToHash()
{
	// enter all names to the hash table
	tjs_uint Count = GetCount();
	tjs_uint i;
	for(i = 0; i < Count; i++)
	{
		ttstr name = GetName(i);
		NormalizeInArchiveStorageName(name);
		Hash.Add(name, i);
		// TVPAddLog(ttstr(TJS_W("arcHash name:")) + name);
		// wprintf(TJS_W("==arcfile %d, name:%ls\n"),i,name.c_str());
	}
	wprintf(TJS_W("total:%d\n"),Count);
}
//---------------------------------------------------------------------------
tTJSBinaryStream * tTVPArchive::CreateStream(const ttstr & name)
{
	if(name.IsEmpty()) return NULL;

	if(!Init)
	{
		Init = true;
		AddToHash();
	}

	tjs_uint *p = Hash.Find(name);
	if(!p) TVPThrowExceptionMessage(TVPStorageInArchiveNotFound,
		name, ArchiveName);

	return CreateStreamByIndex(*p);
}
//---------------------------------------------------------------------------
bool tTVPArchive::IsExistent(const ttstr & name)
{
	if(name.IsEmpty()) return false;

	if(!Init)
	{
		Init = true;
		AddToHash();
	}

	return Hash.Find(name) != NULL;
}
//---------------------------------------------------------------------------
tjs_int tTVPArchive::GetFirstIndexStartsWith(const ttstr & prefix)
{
	// returns first index which have 'prefix' at start of the name.
	// returns -1 if the target is not found.
	// the item must be sorted by ttstr::operator < , otherwise this function
	// will not work propertly.
	tjs_uint total_count = GetCount();
	tjs_int s = 0, e = total_count;
	while(e - s > 1)
	{
		tjs_int m = (e + s) / 2;
		if(!(GetName(m) < prefix))
		{
			// m is after or at the target
			e = m;
		}
		else
		{
			// m is before the target
			s = m;
		}
	}

	// at this point, s or s+1 should point the target.
	// be certain.
	if(s >= (tjs_int)total_count) return -1; // out of the index
	if(GetName(s).StartsWith(prefix)) return s;
	s++;
	if(s >= (tjs_int)total_count) return -1; // out of the index
	if(GetName(s).StartsWith(prefix)) return s;
	return -1;
}
//---------------------------------------------------------------------------


