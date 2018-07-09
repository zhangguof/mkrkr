//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Universal Storage System
//---------------------------------------------------------------------------
#ifndef StorageImplH
#define StorageImplH
//---------------------------------------------------------------------------
#include "StorageIntf.h"
#include "UtilStreams.h"
#include <stdio.h>
#include <boost/filesystem.hpp>
#include "win_def.h"

// #include <objidl.h> // for IStream


//---------------------------------------------------------------------------
// Susie plug-in related
//---------------------------------------------------------------------------
// void TVPLoadArchiveSPI(HINSTANCE inst);
// void TVPUnloadArchiveSPI(HINSTANCE inst);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPLocalFileStream
//---------------------------------------------------------------------------
class tTVPLocalFileStream : public tTJSBinaryStream
{
private:
#ifdef _WIN32
	typedef HANDLE FILEHANDLE;
	FILEHANDLE Handle;
#else
	typedef FILE* FILEHANDLE;
	FILE* Handle;
	boost::filesystem::path path;
#endif

public:
	tTVPLocalFileStream(const ttstr &origname, const ttstr & localname,
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
TJS_EXP_FUNC_DEF(bool, TVPCheckExistentLocalFolder, (const ttstr &name));
	/* name must be an OS's NATIVE folder name */

TJS_EXP_FUNC_DEF(bool, TVPCheckExistentLocalFile, (const ttstr &name));
	/* name must be an OS's NATIVE file name */

TJS_EXP_FUNC_DEF(bool, TVPCreateFolders, (const ttstr &folder));
	/* make folders recursively, like mkdir -p. folder must be OS NATIVE folder name */
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPPluginHolder
//---------------------------------------------------------------------------
/*
	tTVPPluginHolder holds plug-in. if the plug-in is not a local storage,
	plug-in is to be extracted to temporary directory and be held until
	the program done using it.
*/
class tTVPPluginHolder
{
private:
	ttstr LocalName;
	tTVPLocalTempStorageHolder * LocalTempStorageHolder;

public:
	tTVPPluginHolder(const ttstr &aname);
	~tTVPPluginHolder();

	const ttstr & GetLocalName() const;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTVPIStreamAdapter
//---------------------------------------------------------------------------
/*
	this class provides COM's IStream adapter for tTJSBinaryStream
*/
class tTVPIStreamAdapter : public IStream
{
private:
	tTJSBinaryStream *Stream;
	ULONG RefCount;

public:
	tTVPIStreamAdapter(tTJSBinaryStream *ref);
	/*
		the stream passed by argument here is freed by this instance'
		destruction.
	*/

	~tTVPIStreamAdapter();


	// // IUnknown
	// HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
	// 	void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);

	// ISequentialStream
	HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead);
	HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb,
		ULONG *pcbWritten);

	// IStream
	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,
		DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize);
	HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb,
		ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
	HRESULT STDMETHODCALLTYPE Revert(void);
	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb, DWORD dwLockType);
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb, DWORD dwLockType);
	// HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag);
	HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm);

	void ClearStream() {
		Stream = NULL;
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// IStream creator
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(IStream *, TVPCreateIStream, (const ttstr &name, tjs_uint32 flags));
//---------------------------------------------------------------------------





#endif
