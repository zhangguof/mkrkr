#ifndef ArchiveH
#define ArchiveH

#include "tjsNative.h"
#include "tjsHashSearch.h"
#include <vector>


//---------------------------------------------------------------------------
// tTVPArchive base archive class
//---------------------------------------------------------------------------
class tTVPArchive
{
private:
	tjs_uint RefCount;

public:
	//-- constructor
	tTVPArchive(const ttstr & name)
		{ ArchiveName = name; Init = false; RefCount = 1; }
	virtual ~tTVPArchive() { ; }

	//-- AddRef and Release
	void AddRef() { RefCount++; }
	void Release() { if(RefCount == 1) delete this; else RefCount--; }

	//-- must be implemented by delivered class
	virtual tjs_uint GetCount() = 0;
	virtual ttstr GetName(tjs_uint idx) = 0;
		// returned name must be already normalized using NormalizeInArchiveStorageName
		// and the index must be sorted by its name, using ttstr::operator < .
		// this is needed by fast directory search.

	virtual tTJSBinaryStream * CreateStreamByIndex(tjs_uint idx) = 0;

	//-- others, implemented in this class
private:

	tTJSHashTable<ttstr, tjs_uint, tTJSHashFunc<ttstr>, 1024> Hash;
	bool Init;
	ttstr ArchiveName;

public:
	static void NormalizeInArchiveStorageName(ttstr & name);

private:
	void AddToHash();
public:
	tTJSBinaryStream * CreateStream(const ttstr & name);
	bool IsExistent(const ttstr & name);

	tjs_int GetFirstIndexStartsWith(const ttstr & prefix);
		// returns first index which have 'prefix' at start of the name.
};


#endif