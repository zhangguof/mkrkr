
#include "tjsCommHead.h"
#include "MsgIntf.h"

#include "Archive.hpp"


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


