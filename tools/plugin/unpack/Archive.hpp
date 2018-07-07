#ifndef ArchiveH
#define ArchiveH

#include <windows.h>
#include "tp_stub.h"
#include "tjsHashSearch.h"
#include <vector>

//---------------------------------------------------------------------------
// error messages  ( can be localized )
//---------------------------------------------------------------------------
//ttstr TJSNonamedException = TJS_W("No-named exception");
extern const tjs_char* TJSNonamedException;
class eTJS
{
public:
	eTJS() {;}
	eTJS(const eTJS&) {;}
	eTJS& operator= (const eTJS& e) { return *this; }
	virtual ~eTJS() {;}
	virtual const ttstr & GetMessage() const 
	{ return TJSNonamedException; }
};
//---------------------------------------------------------------------------
void TJS_eTJS();
//---------------------------------------------------------------------------
class eTJSError : public eTJS
{
public:
	eTJSError(const ttstr & Msg) :
		Message(Msg) {;}
	const ttstr & GetMessage() const { return Message; }

	void AppendMessage(const ttstr & msg) { Message += msg; }

private:
	ttstr Message;
};

//error
#define ERROR_MSG(name) const tjs_char* name = L#name
#define DEL_ERROR_MSG(name) extern const tjs_char* name;

DEL_ERROR_MSG(TVPCannotUnbindXP3EXE);
DEL_ERROR_MSG(TVPCannotFindXP3Mark);
DEL_ERROR_MSG(TVPInfoTryingToReadXp3VirtualFileSystemInformationFrom);
DEL_ERROR_MSG(TVPReadError);
DEL_ERROR_MSG(TVPUncompressionFailed);
DEL_ERROR_MSG(TVPSpecifiedStorageHadBeenProtected);
DEL_ERROR_MSG(TVPInfoFailed);
DEL_ERROR_MSG(TVPInfoDoneWithContains);
DEL_ERROR_MSG(_TVPXP3ArchiveExtractionFilter);
DEL_ERROR_MSG(TVPXP3ArchiveExtractionFilte);
DEL_ERROR_MSG(TJSWriteError);
DEL_ERROR_MSG(TVPSeekError);
DEL_ERROR_MSG(TVPTruncateError);
DEL_ERROR_MSG(TVPStorageInArchiveNotFound);
DEL_ERROR_MSG(TJSSeekError);
DEL_ERROR_MSG(TJSReadError);




//---------------------------------------------------------------------------
// tTJSBinaryStream base stream class
//---------------------------------------------------------------------------
class tTJSBinaryStream
{
private:
public:
	//-- must implement
	virtual tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence) = 0;
	/* if error, position is not changed */

	//-- optionally to implement
	virtual tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size) = 0;
	/* returns actually read size */

	virtual tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size) = 0;
	/* returns actually written size */

	virtual void TJS_INTF_METHOD SetEndOfStorage();
	// the default behavior is raising a exception
	/* if error, raises exception */

	//-- should re-implement for higher performance
	virtual tjs_uint64 TJS_INTF_METHOD GetSize() = 0;

	virtual ~tTJSBinaryStream() {;}

	tjs_uint64 GetPosition();

	void SetPosition(tjs_uint64 pos);

	void ReadBuffer(void *buffer, tjs_uint read_size);
	void WriteBuffer(const void *buffer, tjs_uint write_size);

	tjs_uint64 ReadI64LE(); // reads little-endian integers
	tjs_uint32 ReadI32LE();
	tjs_uint16 ReadI16LE();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPBinaryStreamAdapter
//---------------------------------------------------------------------------
/*
	this class provides tTJSBinaryStream adapter for IStream
*/
class tTVPBinaryStreamAdapter : public tTJSBinaryStream
{
	typedef tTJSBinaryStream inherited;

private:
	IStream *Stream;

public:
	tTVPBinaryStreamAdapter(IStream *ref);
	/*
		the stream passed by argument here is freed by this instance'
		destruction.
	*/

	~tTVPBinaryStreamAdapter();

	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence);
	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size);
	tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size);
	tjs_uint64 TJS_INTF_METHOD GetSize();
	void TJS_INTF_METHOD SetEndOfStorage();
};
//---------------------------------------------------------------------------
extern tTJSBinaryStream * TVPCreateBinaryStreamAdapter_ex(IStream *refstream);



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
	tTJSHashTable<ttstr, tjs_uint, tTJSHashFunc<ttstr>, 1024 > Hash;
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