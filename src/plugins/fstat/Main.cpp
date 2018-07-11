#include "ncbind/ncbind.hpp"
#include <string>
#include <vector>
using namespace std;
// #include <tchar.h>
// #include <shlobj.h>
// #include <ole2.h>
// #include <shellapi.h> // SHGetFileInfo
#include <boost/filesystem.hpp>
#include "win_def.h"
typedef FILE* HFILE;
typedef std::time_t FILETIME;
// typedef BOOL bool

void CloseHandle(HFILE hFile)
{
	if(hFile)
		::fclose(hFile);
}

extern bool TVPCheckExistentLocalFile(const ttstr &name);
#define INVALID_HANDLE_VALUE NULL

// Date ƒNƒ‰ƒXƒƒ“ƒo
static iTJSDispatch2 *dateClass   = NULL;  // Date ‚ÌƒNƒ‰ƒXƒIƒuƒWƒFƒNƒg
static iTJSDispatch2 *dateSetTime = NULL;  // Date.setTime ƒƒ\ƒbƒh
static iTJSDispatch2 *dateGetTime = NULL;  // Date.getTime ƒƒ\ƒbƒh

static const tjs_nchar * StoragesFstatPreScript	= TJS_N("\
global.FILE_ATTRIBUTE_READONLY = 0x00000001,\
global.FILE_ATTRIBUTE_HIDDEN = 0x00000002,\
global.FILE_ATTRIBUTE_SYSTEM = 0x00000004,\
global.FILE_ATTRIBUTE_DIRECTORY = 0x00000010,\
global.FILE_ATTRIBUTE_ARCHIVE = 0x00000020,\
global.FILE_ATTRIBUTE_NORMAL = 0x00000080,\
global.FILE_ATTRIBUTE_TEMPORARY = 0x00000100;");

NCB_TYPECONV_CAST_INTEGER(tjs_uint64);


/**
 * ƒƒ\ƒbƒh’Ç‰Á—p
 */
class StoragesFstat {
	/**
	 * Win32API‚Ì GetLastError‚ÌƒGƒ‰[ƒƒbƒZ[ƒW‚ð•Ô‚·
	 * @param message ƒƒbƒZ[ƒWŠi”[æ
	 */
	static void getLastError(ttstr &message) {
		// LPVOID lpMessageBuffer;
		// FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		// 			   NULL, GetLastError(),
		// 			   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		// 			   (LPWSTR)&lpMessageBuffer, 0, NULL);
		// message = ((tjs_char*)lpMessageBuffer);
		// LocalFree(lpMessageBuffer);
		message = TJS_W("getLastError!!");
	}

	/**
	 * ƒtƒ@ƒCƒ‹Žž‚ð Date ƒNƒ‰ƒX‚É‚µ‚Ä•Û‘¶
	 * @param store Ši”[æ
	 * @param filetime ƒtƒ@ƒCƒ‹Žž
	 */
	static void storeDate(tTJSVariant &store, const std::time_t &filetime, iTJSDispatch2 *objthis)
	{
		// ƒtƒ@ƒCƒ‹¶¬Žž
		tjs_uint64 ft = filetime;
		if (ft > 0) {
			iTJSDispatch2 *obj;
			if (TJS_SUCCEEDED(dateClass->CreateNew(0, NULL, NULL, &obj, 0, NULL, objthis))) {
				// UNIX TIME ‚É•ÏŠ·
				tjs_int64 unixtime = ft;
				tTJSVariant time(unixtime);
				tTJSVariant *param[] = { &time };
				dateSetTime->FuncCall(0, NULL, NULL, NULL, 1, param, obj);
				store = tTJSVariant(obj, obj);
				obj->Release();
			}
		}
	}
	/**
	 * Date ƒNƒ‰ƒX‚ÌŽž‚ðƒtƒ@ƒCƒ‹Žž‚É•ÏŠ·
	 * @param restore  ŽQÆæiDateƒNƒ‰ƒXƒCƒ“ƒXƒ^ƒ“ƒXj
	 * @param filetime ƒtƒ@ƒCƒ‹ŽžŒ‹‰ÊŠi”[æ
	 * @return Žæ“¾‚Å‚«‚½‚©‚Ç‚¤‚©
	 */
	static bool restoreDate(tTJSVariant &restore, std::time_t &filetime)
	{
		if (restore.Type() != tvtObject) return false;
		iTJSDispatch2 *date = restore.AsObjectNoAddRef();
		if (!date) return false;
		tTJSVariant result;
		if (dateGetTime->FuncCall(0, NULL, NULL, &result, 0, NULL, date) != TJS_S_OK) return false;
		tjs_uint64 ft = result.AsInteger();
		filetime = ft;
		return true;
	}

	/**
	 * ƒpƒX‚ðƒ[ƒJƒ‹‰»‚·‚é•––”ö‚Ì\‚ðíœ
	 * @param path ƒpƒX–¼
	 */
	static void getLocalName(ttstr &path) {
		TVPGetLocalName(path);
		if (path.GetLastChar() == TJS_W('\\')||
		    path.GetLastChar()==TJS_W('/')) 
		{
			tjs_int i,len = path.length();
			tjs_char* tmp = new tjs_char[len];
			const tjs_char* dp = path.c_str();
			for (i=0,len--; i<len; i++) tmp[i] = dp[i];
			tmp[i] = 0;
			path = tmp;
			delete[] tmp;
		}
	}
	/**
	 * ƒ[ƒJƒ‹ƒpƒX‚Ì—L–³”»’è
	 * @param in  path  ƒpƒX–¼
	 * @param out local ƒ[ƒJƒ‹ƒpƒX
	 * @return ƒ[ƒJƒ‹ƒpƒX‚ª‚ ‚éê‡‚Ítrue
	 */
	static bool getLocallyAccessibleName(const ttstr &path, ttstr *local = NULL) {
		bool r = false;
		if (local) {
			*local = TVPGetLocallyAccessibleName(path);
			r = ! local->IsEmpty();
		} else {
			ttstr local(TVPGetLocallyAccessibleName(path));
			r = ! local.IsEmpty();
		}
		return r;
	}

	/**
	 * ƒtƒ@ƒCƒ‹ƒnƒ“ƒhƒ‹‚ðŽæ“¾
	 * @param filename ƒtƒ@ƒCƒ‹–¼iƒ[ƒJƒ‹–¼‚Å‚ ‚é‚±‚Æj
	 * @param iswrite “Ç‚Ý‘‚«‘I‘ð
	 * @param out out_isdir ƒfƒBƒŒƒNƒgƒŠ‚©‚Ç‚¤‚©
	 * @return ƒtƒ@ƒCƒ‹ƒnƒ“ƒhƒ‹
	 */
	static FILE* _getFileHandle(ttstr const &filename, bool iswrite, bool *out_isdir = 0) {
		// DWORD attr = GetFileAttributes(filename.c_str());
		boost::filesystem::path p(filename.c_str());
		bool isdir = boost::filesystem::is_directory(p);
		if (out_isdir) *out_isdir = isdir;
		FILE* hFile;
		if (iswrite) {
			hFile = ::fopen(p.c_str(),"wb");
			// hFile = CreateFile(filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL ,
			// 				   OPEN_EXISTING,    isdir ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL , NULL);
		} else {
			hFile = ::fopen(p.c_str(),"rb");
		}
		return hFile;
	}
	/**
	 * ƒtƒ@ƒCƒ‹‚Ìƒ^ƒCƒ€ƒXƒ^ƒ“ƒv‚ðŽæ“¾‚·‚é
	 * @param filename ƒtƒ@ƒCƒ‹–¼iƒ[ƒJƒ‹–¼‚Å‚ ‚é‚±‚Æj
	 * @param ctime ì¬Žž
	 * @param atime ƒAƒNƒZƒXŽž
	 * @param mtime •ÏXŽž
	 * @param size  ƒtƒ@ƒCƒ‹ƒTƒCƒY
	 * @return 0:Ž¸”s 1:ƒtƒ@ƒCƒ‹ 2:ƒtƒHƒ‹ƒ_
	 */
	static int getFileTime(ttstr const &filename, tTJSVariant &ctime, tTJSVariant &atime, tTJSVariant &mtime, tTJSVariant *size = 0)
	{
		bool isdir = false;
		FILE* hFile = _getFileHandle(filename, false, &isdir);
		if (hFile == NULL) return 0;
		::fclose(hFile);

		boost::filesystem::path p(filename.c_str());

		if (!isdir && size != 0) {

			// LARGE_INTEGER fsize;
			// if (GetFileSizeEx(hFile, &fsize))
			// 	*size = (tjs_int64)fsize.QuadPart;
			*size = (tjs_int64)file_size(p);


		}
		std::time_t ftc, fta, ftm;
		ftc = fta = 0;
		ftm = last_write_time(p);
		{
			storeDate(ctime, ftc, NULL);
			storeDate(atime, fta, NULL);
			storeDate(mtime, ftm, NULL);
		}
		// CloseHandle(hFile);
		
		return isdir ? 2 : 1;
	}
	/**
	 * ƒtƒ@ƒCƒ‹‚Ìƒ^ƒCƒ€ƒXƒ^ƒ“ƒv‚ðÝ’è‚·‚é
	 * @param filename ƒtƒ@ƒCƒ‹–¼iƒ[ƒJƒ‹–¼‚Å‚ ‚é‚±‚Æj
	 * @param ctime ì¬Žž
	 * @param mtime •ÏXŽž
	 * @param atime ƒAƒNƒZƒXŽž
	 * @return 0:Ž¸”s 1:ƒtƒ@ƒCƒ‹ 2:ƒtƒHƒ‹ƒ_
	 */
	static int setFileTime(ttstr const &filename, tTJSVariant &ctime, tTJSVariant &atime, tTJSVariant &mtime)
	{
		bool isdir = false;
		HFILE hFile = _getFileHandle(filename, true, &isdir);
		if (hFile == NULL) return 0;

		// std::time_t c, a, m;
		// bool hasC = restoreDate(ctime, c);
		// bool hasA = restoreDate(atime, a);
		// bool hasM = restoreDate(mtime, m);

		// BOOL r = SetFileTime(hFile, hasC?&c:0, hasA?&a:0, hasM?&m:0);
		int r = 1;
		// if (r == 0) {
		// 	ttstr mes;
		// 	getLastError(mes);
		// 	TVPAddLog(ttstr(TJS_W("setFileTime : ")) + filename + TJS_W(":") + mes);
		// }
		::fclose(hFile);
		return (r == 0) ? 0 : isdir ? 2 : 1;
	}
	static tjs_error _getTime(tTJSVariant *result, tTJSVariant const *param, bool chksize) {
		// ŽÀƒtƒ@ƒCƒ‹‚Åƒ`ƒFƒbƒN
		ttstr filename = TVPNormalizeStorageName(param->AsStringNoAddRef());
		getLocalName(filename);
		tTJSVariant size, ctime, atime, mtime;
		int sel = getFileTime(filename, ctime, atime, mtime, chksize ? &size : 0);
		if (sel > 0) {
			if (result) {
				iTJSDispatch2 *dict = TJSCreateDictionaryObject();
				if (dict != NULL) {
					if (chksize && sel == 1) dict->PropSet(TJS_MEMBERENSURE, L"size",  NULL, &size, dict);
					dict->PropSet(TJS_MEMBERENSURE, L"mtime", NULL, &mtime, dict);
					dict->PropSet(TJS_MEMBERENSURE, L"ctime", NULL, &ctime, dict);
					dict->PropSet(TJS_MEMBERENSURE, L"atime", NULL, &atime, dict);
					*result = dict;
					dict->Release();
				}
			}
			return TJS_S_OK;
		}

		TVPThrowExceptionMessage((ttstr(TJS_W("cannot open : ")) + param->GetString()).c_str());
		return TJS_S_OK;
	}
public:
	StoragesFstat(){};

	static void clearStorageCaches() {
		TVPClearStorageCaches();
	}
	
	/**
	 * Žw’è‚³‚ê‚½ƒtƒ@ƒCƒ‹‚Ìî•ñ‚ðŽæ“¾‚·‚é
	 * @param filename ƒtƒ@ƒCƒ‹–¼
	 * @return ƒTƒCƒYEŽžŽ«‘
	 * ¦ƒA[ƒJƒCƒu“àƒtƒ@ƒCƒ‹‚ÍƒTƒCƒY‚Ì‚Ý•Ô‚·
	 */
	static tjs_error TJS_INTF_METHOD fstat(tTJSVariant *result,
										   tjs_int numparams,
										   tTJSVariant **param,
										   iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;

		ttstr filename = TVPGetPlacedPath(*param[0]);
		if (filename.length() > 0 && !getLocallyAccessibleName(filename)) {
			// ƒA[ƒJƒCƒu“àƒtƒ@ƒCƒ‹
			// IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
			// if (in) {
			// 	STATSTG stat;
			// 	in->Stat(&stat, STATFLAG_NONAME);
			// 	tTJSVariant size((tjs_int64)stat.cbSize.QuadPart);
			// 	if (result) {
			// 		iTJSDispatch2 *dict;
			// 		if ((dict = TJSCreateDictionaryObject()) != NULL) {
			// 			dict->PropSet(TJS_MEMBERENSURE, L"size",  NULL, &size, dict);
			// 			*result = dict;
			// 			dict->Release();
			// 		}
			// 	}
			// 	in->Release();
			// 	return TJS_S_OK;
			// }
		}
		return _getTime(result, param[0], true);
	}
	/**
	 * Žw’è‚³‚ê‚½ƒtƒ@ƒCƒ‹‚Ìƒ^ƒCƒ€ƒXƒ^ƒ“ƒvî•ñ‚ðŽæ“¾‚·‚éiƒA[ƒJƒCƒu“à•s‰Âj
	 * @param filename ƒtƒ@ƒCƒ‹–¼
	 * @param dict     ŽžŽ«‘
	 * @return ¬Œ÷‚µ‚½‚©‚Ç‚¤‚©
	 */
	static tjs_error TJS_INTF_METHOD getTime(tTJSVariant *result,
											 tjs_int numparams,
											 tTJSVariant **param,
											 iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		return _getTime(result, param[0], false);
	}
	/**
	 * Žw’è‚³‚ê‚½ƒtƒ@ƒCƒ‹‚Ìƒ^ƒCƒ€ƒXƒ^ƒ“ƒvî•ñ‚ðÝ’è‚·‚é
	 * @param filename ƒtƒ@ƒCƒ‹–¼
	 * @param dict     ŽžŽ«‘
	 * @return ¬Œ÷‚µ‚½‚©‚Ç‚¤‚©
	 */
	static tjs_error TJS_INTF_METHOD setTime(tTJSVariant *result,
											 tjs_int numparams,
											 tTJSVariant **param,
											 iTJSDispatch2 *objthis) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;

		ttstr filename = TVPNormalizeStorageName(param[0]->AsStringNoAddRef());
		getLocalName(filename);
		tTJSVariant size, ctime, atime, mtime;
		iTJSDispatch2 *dict = param[1]->AsObjectNoAddRef();
		if (dict != NULL) {
			dict->PropGet(0, L"ctime", NULL, &ctime, dict);
			dict->PropGet(0, L"atime", NULL, &atime, dict);
			dict->PropGet(0, L"mtime", NULL, &mtime, dict);
		}
		int sel = setFileTime(filename, ctime, atime, mtime);
		if (result) *result = (sel > 0);

		return TJS_S_OK;
	}

	/**
	 * XV“úŽžŽæ“¾EÝ’èiDate‚ðŒo—R‚µ‚È‚¢‚‘¬”Åj
	 * @param target ‘ÎÛ
	 * @param time ŽžŠÔi64bit FILETIME”j
	 */
	static tjs_uint64 getLastModifiedFileTime(ttstr target) {
		ttstr filename = TVPNormalizeStorageName(target);
		getLocalName(filename);
		HFILE hFile = _getFileHandle(filename, false);
		FILETIME ft;
		if (hFile == INVALID_HANDLE_VALUE) return 0;
		CloseHandle(hFile);
		boost::filesystem::path p(filename.c_str());

		tjs_uint64 ret = boost::filesystem::last_write_time(p);
		return ret;
	}
	static bool setLastModifiedFileTime(ttstr target, tjs_uint64 time) {
		ttstr filename = TVPNormalizeStorageName(target);
		getLocalName(filename);
		HFILE hFile = _getFileHandle(filename, true);
		if (hFile == INVALID_HANDLE_VALUE) return false;
		FILETIME ft = time;
		// ft.dwHighDateTime = (time >> 32) & 0xFFFFFFFF;
		// ft.dwLowDateTime  =  time        & 0xFFFFFFFF;
		// bool rs = !! SetFileTime(hFile, 0, 0, &ft);
		bool rs = true;
		CloseHandle(hFile);
		return rs;
	}

	/**
	 * ‹g—¢‹g—¢‚ÌƒXƒgƒŒ[ƒW‹óŠÔ’†‚Ìƒtƒ@ƒCƒ‹‚ð’Šo‚·‚é
	 * @param src •Û‘¶Œ³ƒtƒ@ƒCƒ‹
	 * @param dest •Û‘¶æƒtƒ@ƒCƒ‹
	 */
	static void exportFile(ttstr filename, ttstr storename) {
		TVPAddLog("Not implement! exportFile");
		// IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
		// if (in) {
		// 	IStream *out = TVPCreateIStream(storename, TJS_BS_WRITE);
		// 	if (out) {
		// 		BYTE buffer[1024*16];
		// 		DWORD size;
		// 		while (in->Read(buffer, sizeof buffer, &size) == S_OK && size > 0) {			
		// 			out->Write(buffer, size, &size);
		// 		}
		// 		out->Release();
		// 		in->Release();
		// 	} else {
		// 		in->Release();
		// 		TVPThrowExceptionMessage((ttstr(TJS_W("cannot open storefile: ")) + storename).c_str());
		// 	}
		// } else {
		// 	TVPThrowExceptionMessage((ttstr(TJS_W("cannot open readfile: ")) + filename).c_str());
		// }
	}

	/**
	 * ‹g—¢‹g—¢‚ÌƒXƒgƒŒ[ƒW‹óŠÔ’†‚ÌŽw’èƒtƒ@ƒCƒ‹‚ðíœ‚·‚éB
	 * @param file íœ‘ÎÛƒtƒ@ƒCƒ‹
	 * @return ŽÀÛ‚Éíœ‚³‚ê‚½‚ç true
	 * ŽÀƒtƒ@ƒCƒ‹‚ª‚ ‚éê‡‚Ì‚Ýíœ‚³‚ê‚Ü‚·
	 */
	static bool deleteFile(const tjs_char *file) {
		bool r = false;
		ttstr filename(TVPGetLocallyAccessibleName(file));
		if (filename.length()) {
			r	= TVPRemoveFile(filename);
			if (r == false) {
				ttstr mes = TJS_W("remove file error!");
				TVPAddLog(ttstr(TJS_W("deleteFile : ")) + filename + TJS_W(" : ") + mes);
			} else {
				// íœ‚É¬Œ÷‚µ‚½ê‡‚ÍƒXƒgƒŒ[ƒWƒLƒƒƒbƒVƒ…‚ðƒNƒŠƒA
				TVPClearStorageCaches();
			}
		}
		return r;
	}

	/**
	 * ‹g—¢‹g—¢‚ÌƒXƒgƒŒ[ƒW‹óŠÔ’†‚ÌŽw’èƒtƒ@ƒCƒ‹‚ÌƒTƒCƒY‚ð•ÏX‚·‚é(Ø‚èŽÌ‚Ä‚é)
	 * @param file ƒtƒ@ƒCƒ‹
	 * @param size Žw’èƒTƒCƒY
	 * @return ƒTƒCƒY•ÏX‚Å‚«‚½‚ç true
	 * ŽÀƒtƒ@ƒCƒ‹‚ª‚ ‚éê‡‚Ì‚Ýˆ—‚³‚ê‚Ü‚·
	 */
	static bool truncateFile(const tjs_char *file, tjs_int size) {
		TVPAddLog("Not implement! truncateFile!");
		bool r = false;
		// ttstr filename(TVPGetLocallyAccessibleName(TVPGetPlacedPath(file)));
		// if (filename.length()) {
		// 	HANDLE hFile = _getFileHandle(filename, true);
		// 	if (hFile != INVALID_HANDLE_VALUE) {
		// 		LARGE_INTEGER ofs;
		// 		ofs.QuadPart = size;
		// 		if (SetFilePointerEx(hFile, ofs, NULL, FILE_BEGIN) &&
		// 			SetEndOfFile(hFile)) {
		// 			r = true;
		// 		} else {
		// 			ttstr mes;
		// 			getLastError(mes);
		// 			TVPAddLog(ttstr(TJS_W("truncateFile : ")) + filename + TJS_W(" : ") + mes);
		// 		}
		// 		CloseHandle(hFile);
		// 	}
		// }
		return !! r;
	}
	
	/**
	 * Žw’èƒtƒ@ƒCƒ‹‚ðˆÚ“®‚·‚éB
	 * @param fromFile ˆÚ“®‘ÎÛƒtƒ@ƒCƒ‹
	 * @param toFile ˆÚ“®æƒpƒX
	 * @return ŽÀÛ‚ÉˆÚ“®‚³‚ê‚½‚ç true
	 * ˆÚ“®‘ÎÛƒtƒ@ƒCƒ‹‚ªŽÀÝ‚µAˆÚ“®æƒpƒX‚Éƒtƒ@ƒCƒ‹‚ª–³‚¢ê‡‚Ì‚ÝˆÚ“®‚³‚ê‚Ü‚·
	 */
	static bool moveFile(const tjs_char *from, const tjs_char *to) {
		bool r = false;
		TVPAddLog("Not Implement! moveFile!");

		// ttstr fromFile(TVPGetLocallyAccessibleName(from));
		// ttstr   toFile(TVPGetLocallyAccessibleName(to));
		// if (fromFile.length()
		// 	&& toFile.length()) {
		// 	r	= MoveFile(fromFile.c_str(), toFile.c_str());
		// 	if (r == FALSE) {
		// 		ttstr mes;
		// 		getLastError(mes);
		// 		TVPAddLog(ttstr(TJS_W("moveFile : ")) + fromFile + ", " + toFile + TJS_W(" : ") + mes);
		// 	} else {
		// 		TVPClearStorageCaches();
		// 	}
		// }
		return !! r;
	}

	/**
	 * Žw’èƒfƒBƒŒƒNƒgƒŠ‚Ìƒtƒ@ƒCƒ‹ˆê——‚ðŽæ“¾‚·‚é
	 * @param dir ƒfƒBƒŒƒNƒgƒŠ–¼
	 * @return ƒtƒ@ƒCƒ‹–¼ˆê——‚ªŠi”[‚³‚ê‚½”z—ñ
	 */
	static tTJSVariant dirlist(tjs_char const *dir) {
		return _dirlist(dir, &setDirListFile);
	}

	/**
	 * Žw’èƒfƒBƒŒƒNƒgƒŠ‚Ìƒtƒ@ƒCƒ‹ˆê——‚ÆÚ×î•ñ‚ðŽæ“¾‚·‚é
	 * @param dir ƒfƒBƒŒƒNƒgƒŠ–¼
	 * @return ƒtƒ@ƒCƒ‹î•ñˆê——‚ªŠi”[‚³‚ê‚½”z—ñ
	 *         [ %[ name:ƒtƒ@ƒCƒ‹–¼, size, attrib, mtime, atime, ctime ], ... ]
	 * dirlist‚Æˆá‚¢name‚É‚¨‚¢‚ÄƒtƒHƒ‹ƒ_‚Ìê‡‚Ì––”ö"/"’Ç‰Á‚ª‚È‚¢‚Ì‚Å’ˆÓ(attrib‚Å”»’è‚Ì‚±‚Æ)
	 */
	static tTJSVariant dirlistEx(tjs_char const *dir) {
		return _dirlist(dir, &setDirListInfo);
	}

	typedef bool (*DirListCallback)(iTJSDispatch2 *array, tjs_int count, ttstr const &file, void const *data);
private:
	static tTJSVariant _dirlist(ttstr dir, DirListCallback cb)
	{
		assert(false);//not implenment;
		// OSƒlƒCƒeƒBƒu‚È•\Œ»‚É•ÏŠ·
		dir = TVPNormalizeStorageName(dir);
		if (dir.GetLastChar() != TJS_W('/')) {
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		TVPGetLocalName(dir);

		// Array ƒNƒ‰ƒX‚ÌƒIƒuƒWƒFƒNƒg‚ðì¬
		iTJSDispatch2 * array = TJSCreateArrayObject();
		tTJSVariant result;

		// try {
		// 	ttstr wildcard = dir + "*.*";
		// 	void data;
		// 	HANDLE handle = FindFirstFile(wildcard.c_str(), &data);
		// 	if (handle != INVALID_HANDLE_VALUE) {
		// 		tjs_int count = 0;
		// 		do {
		// 			ttstr file = data.cFileName;
		// 			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		// 				// ƒfƒBƒŒƒNƒgƒŠ‚Ìê‡‚ÍÅŒã‚É / ‚ð‚Â‚¯‚é
		// 				file += "/";
		// 			}
		// 			if ((*cb)(array, count, file, &data)) count++;
		// 		} while(FindNextFile(handle, &data));
		// 		FindClose(handle);
		// 	} else {
		// 		TVPThrowExceptionMessage(TJS_W("Directory not found."));
		// 	}
		// 	result = tTJSVariant(array, array);
		// 	array->Release();
		// } catch(...) {
		// 	array->Release();
		// 	throw;
		// }

		return result;
	}
	static bool setDirListFile(iTJSDispatch2 *array, tjs_int count, ttstr const &file, void const *data) {
		// [dirlist] ”z—ñ‚É’Ç‰Á‚·‚é
		tTJSVariant val(file);
		array->PropSetByNum(0, count, &val, array);
		return true;
	}
	static bool setDirListInfo(iTJSDispatch2 *array, tjs_int count, ttstr const &file, void const *data) {
		// [dirlistEx] ”z—ñ‚É’Ç‰Á‚·‚é
		// iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		// if (dict != NULL) try {
		// 	{
		// 		ttstr fname = data->cFileName;
		// 		tTJSVariant name = fname;
		// 		dict->PropSet(TJS_MEMBERENSURE, L"name", NULL, &name, dict);
		// 	} {
		// 		tjs_int64 fsize = data->nFileSizeHigh;
		// 		fsize <<= 32;
		// 		fsize  |= data->nFileSizeLow;
		// 		tTJSVariant size = fsize;
		// 		dict->PropSet(TJS_MEMBERENSURE, L"size", NULL, &size, dict);
		// 	} {
		// 		tTJSVariant attrib = (tjs_int)data->dwFileAttributes;
		// 		dict->PropSet(TJS_MEMBERENSURE, L"attrib", NULL, &attrib, dict);
		// 	} {
		// 		tTJSVariant ctime, atime, mtime;
		// 		storeDate(ctime, data->ftCreationTime,   NULL);
		// 		storeDate(atime, data->ftLastAccessTime, NULL);
		// 		storeDate(mtime, data->ftLastWriteTime,  NULL);
		// 		dict->PropSet(TJS_MEMBERENSURE, L"mtime", NULL, &mtime, dict);
		// 		dict->PropSet(TJS_MEMBERENSURE, L"ctime", NULL, &ctime, dict);
		// 		dict->PropSet(TJS_MEMBERENSURE, L"atime", NULL, &atime, dict);
		// 	}
		// 	tTJSVariant val(dict, dict);
		// 	array->PropSetByNum(0, count, &val, array);
		// 	dict->Release();
		// } catch(...) {
		// 	dict->Release();
		// 	throw;
		// }
		return true;
	}

public:

	/**
	 * Žw’èƒfƒBƒŒƒNƒgƒŠ‚ðíœ‚·‚é
	 * @param dir ƒfƒBƒŒƒNƒgƒŠ–¼
	 * @return ŽÀÛ‚Éíœ‚³‚ê‚½‚ç true
	 * ’†‚Éƒtƒ@ƒCƒ‹‚ª–³‚¢ê‡‚Ì‚Ýíœ‚³‚ê‚Ü‚·
	 */
	static bool removeDirectory(ttstr dir) {

		if (dir.GetLastChar() != TJS_W('/')) {
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}

		// OSƒlƒCƒeƒBƒu‚È•\Œ»‚É•ÏŠ·
		dir = TVPNormalizeStorageName(dir);
		TVPGetLocalName(dir);

		bool r = TVPRemoveFolder(dir);
		if(r == false) {
			ttstr mes = TJS_W("remove error!");
			TVPAddLog(ttstr(TJS_W("removeDirectory : ")) + dir + TJS_W(" : ") + mes);
		}
		return r;
	}

	/**
	 * ƒfƒBƒŒƒNƒgƒŠ‚Ìì¬
	 * @param dir ƒfƒBƒŒƒNƒgƒŠ–¼
	 * @return ŽÀÛ‚Éì¬‚Å‚«‚½‚ç true
	 */
	static bool createDirectory(ttstr dir)
	{
		if(dir.GetLastChar() != TJS_W('/'))
		{
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		dir	= TVPNormalizeStorageName(dir);
		TVPGetLocalName(dir);
		bool r = TVPCreateFolders(dir);
		if (r == false) {
			ttstr mes = TJS_W("createDirectory error!");
			TVPAddLog(ttstr(TJS_W("createDirectory : ")) + dir + TJS_W(" : ") + mes);
		}
		return r;
	}

	/**
	 * ƒfƒBƒŒƒNƒgƒŠ‚Ìì¬
	 * @param dir ƒfƒBƒŒƒNƒgƒŠ–¼
	 * @return ŽÀÛ‚Éì¬‚Å‚«‚½‚ç true
	 */
	static bool createDirectoryNoNormalize(ttstr dir)
	{
		if(dir.GetLastChar() != TJS_W('/'))
		{
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		TVPGetLocalName(dir);
		bool r = TVPCreateFolders(dir);
		if (r == false) {
			ttstr mes = TJS_W("createDirectory error!");
			TVPAddLog(ttstr(TJS_W("createDirectory : ")) + dir + TJS_W(" : ") + mes);
		}
		return r;
	}

	/**
	 * ƒJƒŒƒ“ƒgƒfƒBƒŒƒNƒgƒŠ‚Ì•ÏX
	 * @param dir ƒfƒBƒŒƒNƒgƒŠ–¼
	 * @return ŽÀÛ‚Éì¬‚Å‚«‚½‚ç true
	 */
	static bool changeDirectory(ttstr dir)
	{
		if(dir.GetLastChar() != TJS_W('/'))
		{
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		dir	= TVPNormalizeStorageName(dir);
		TVPGetLocalName(dir);
		bool r = isExistentDirectory(dir);
		if (r == false) {
			ttstr mes = TJS_W("SetCurrentDirectory error!");
			TVPAddLog(ttstr(TJS_W("changeDirectory : ")) + dir + TJS_W(" : ") + mes);
		}
		TVPSetCurrentDirectory(dir);
		return r;
	}
	
	/**
	 * ƒtƒ@ƒCƒ‹‚Ì‘®«‚ðÝ’è‚·‚é
	 * @param filename ƒtƒ@ƒCƒ‹/ƒfƒBƒŒƒNƒgƒŠ–¼
	 * @param attr Ý’è‚·‚é‘®«
	 * @return ŽÀÛ‚É•ÏX‚Å‚«‚½‚ç true
	 */
	static bool setFileAttributes(ttstr filename, DWORD attr)
	{
		return true;
		// filename	= TVPNormalizeStorageName(filename);
		// TVPGetLocalName(filename);

		// attr	= attr & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN |
		// 	FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY);

		// DWORD	orgattr = GetFileAttributes(filename.c_str());

		// return SetFileAttributes(filename.c_str(), orgattr | attr) == TRUE;
	}

	/**
	 * ƒtƒ@ƒCƒ‹‚Ì‘®«‚ð‰ðœ‚·‚é
	 * @param filename ƒtƒ@ƒCƒ‹/ƒfƒBƒŒƒNƒgƒŠ–¼
	 * @param attr ‰ðœ‚·‚é‘®«
	 * @return ŽÀÛ‚É•ÏX‚Å‚«‚½‚ç true
	 */
	static bool resetFileAttributes(ttstr filename, DWORD attr)
	{
		return true;
		// filename	= TVPNormalizeStorageName(filename);
		// TVPGetLocalName(filename);

		// attr	= attr & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN |
		// 	FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY);

		// DWORD	orgattr = GetFileAttributes(filename.c_str());

		// return SetFileAttributes(filename.c_str(), orgattr & ~attr) == TRUE;
	}


	static DWORD getFileAttributes(ttstr filename)
	{
		filename	= TVPNormalizeStorageName(filename);
		TVPGetLocalName(filename);

		return 0;
	}


	static tjs_error TJS_INTF_METHOD selectDirectory(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		if(numparams < 1)
			return TJS_E_BADPARAMCOUNT;

		// iTJSDispatch2*	tmp;
		// tTJSVariant		val;
		// BROWSEINFO		bi;
		// ITEMIDLIST*		rootidl	= NULL;
		// tjs_char		folder[2048+1];
		// memset(&bi, 0, sizeof(bi));

		// //	HWND
		// iTJSDispatch2*	elm	= param[0]->AsObjectNoAddRef();
		// if(elm->IsValid(0, L"window", NULL, elm) == TJS_S_TRUE &&
		// 	TJS_SUCCEEDED(elm->PropGet(0, L"window", NULL, &val, elm)))
		// {
		// 	HWND	owner	= NULL;
		// 	if(val.Type() != tvtVoid)
		// 	{
		// 		tmp	= val.AsObjectNoAddRef();
		// 		if(TJS_SUCCEEDED(tmp->PropGet(0, L"HWND", NULL, &val, tmp)))
		// 			owner	= (HWND)val.AsInteger();
		// 	}
		// 	bi.hwndOwner	= owner != NULL ? owner : TVPGetApplicationWindowHandle();
		// }
		// else
		// 	bi.hwndOwner	= NULL;

		// //	title
		// if(elm->IsValid(0, L"title", NULL, elm) == TJS_S_TRUE &&
		// 	TJS_SUCCEEDED(elm->PropGet(0, L"title", NULL, &val, elm)))
		// {
		// 	ttstr	title	= val.AsStringNoAddRef();
		// 	bi.lpszTitle	= title.c_str();
		// }
		// else
		// 	bi.lpszTitle	= NULL;

		// //	name
		// bi.pszDisplayName	= NULL;
		// bi.ulFlags	= BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		// if(elm->IsValid(0, L"name", NULL, elm) == TJS_S_TRUE &&
		// 	TJS_SUCCEEDED(elm->PropGet(0, L"name", NULL, &val, elm)) &&
		// 	!val.NormalCompare(ttstr(L"")))
		// {
		// 	ttstr	name	= TVPNormalizeStorageName(val.AsStringNoAddRef());
		// 	TVPGetLocalName(name);
		// 	_tcscpy_s(folder, 2048, name.c_str());
		// }
		// else
		// 	folder[0]	= 0;

		// //	root dir
		// if(elm->IsValid(0, L"rootDir", NULL, elm) == TJS_S_TRUE &&
		// 	TJS_SUCCEEDED(elm->PropGet(0, L"rootDir", NULL, &val, elm)))
		// {
		// 	ttstr	rootDir	= TVPNormalizeStorageName(val.AsStringNoAddRef());
		// 	TVPGetLocalName(rootDir);
		// 	rootidl	= Path2ITEMIDLIST(rootDir.c_str());
		// }
		// bi.pidlRoot	= rootidl;

		// bi.lpfn		= SelectDirectoryCallBack;
		// bi.lParam	= (LPARAM)folder;

		// ITEMIDLIST*	pidl;
		// if((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		// {
		// 	if(::SHGetPathFromIDList(pidl, folder) != TRUE)
		// 	{
		// 		if(result) *result = 0;
		// 	}
		// 	else
		// 	{
		// 		if(result) *result = TJS_S_TRUE;
		// 		val	= folder;
		// 		val = TVPNormalizeStorageName(val);
		// 		elm->PropSet(TJS_MEMBERENSURE, L"name", NULL, &val, elm);
		// 	}
		// 	FreeITEMIDLIST(pidl);
		// }
		// else
		// 	if(result) *result	= 0;
		// FreeITEMIDLIST(rootidl);

		return TJS_S_OK;
	}


	static int isExistentDirectory(ttstr dir)
	{
		if(dir.GetLastChar() != TJS_W('/'))
		{
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		dir	= TVPNormalizeStorageName(dir);
		TVPGetLocalName(dir);
		// printf("get localname:%ls\n",dir.c_str());

		return TVPCheckExistentLocalFile(dir);

	}


	static bool copyFile(const tjs_char *from, const tjs_char *to, bool failIfExist)
	{
		ttstr fromFile(TVPGetLocallyAccessibleName(TVPGetPlacedPath(from)));
		ttstr toFile  (TVPGetLocallyAccessibleName(TVPNormalizeStorageName(to)));
		return _copyFile(fromFile, toFile, failIfExist);
	}
private:
	static bool _copyFile(const ttstr &from, const ttstr &to, bool failIfExist)
	{
		if(from.length() && to.length())
		{
			// if(CopyFile(from.c_str(), to.c_str(), failIfExist)) {
			// 	TVPClearStorageCaches();
			// 	return true;
			// }
			//not implenment
			assert(false);
			return false;
		}
		return false;
	}
public:

	/**
	 * ƒpƒX‚Ì³‹K‰»‚ðs‚í‚¸‹g—¢‹g—¢‚ÌƒXƒgƒŒ[ƒW‹óŠÔ’†‚ÌŽw’èƒtƒ@ƒCƒ‹‚ðƒRƒs[‚·‚é
	 * @param from ƒRƒs[Œ³ƒtƒ@ƒCƒ‹
	 * @param to ƒRƒs[æƒtƒ@ƒCƒ‹
	 * @param failIfExist ƒtƒ@ƒCƒ‹‚ª‘¶Ý‚·‚é‚Æ‚«‚ÉŽ¸”s‚·‚é‚È‚ç tureAã‘‚«‚·‚é‚È‚ç false
	 * @return ŽÀÛ‚ÉˆÚ“®‚Å‚«‚½‚ç true
	 */
	static bool copyFileNoNormalize(const tjs_char *from, const tjs_char *to, bool failIfExist)
	{
		ttstr fromFile(TVPGetLocallyAccessibleName(TVPGetPlacedPath(from)));
		ttstr toFile(to);
		if(toFile.length())
		{
			// ¦Žw’èŽŸ‘æ‚Å—áŠO‚ð”­¶‚³‚¹‚é‚½‚ßTVPGetLocallyAccessibleName‚ÍŽg‚í‚È‚¢
			TVPGetLocalName(toFile);
			return _copyFile(fromFile, toFile, failIfExist);
		}
		return false;
	}

	/**
	 * ƒpƒX‚Ì³‹K‰»‚ðs‚È‚í‚¸AautoPath‚©‚ç‚ÌŒŸõ‚às‚È‚í‚¸‚É
	 * ƒtƒ@ƒCƒ‹‚Ì‘¶ÝŠm”F‚ðs‚¤
	 * @param fileame ƒtƒ@ƒCƒ‹ƒpƒX
	 * @return ƒtƒ@ƒCƒ‹‚ª‘¶Ý‚µ‚½‚çtrue
	 */
	static bool isExistentStorageNoSearchNoNormalize(ttstr filename) 
	{
		return TVPIsExistentStorageNoSearchNoNormalize(filename);
	}

	/**
	 * •\Ž¦–¼Žæ“¾
	 * @param fileame ƒtƒ@ƒCƒ‹ƒpƒX
	 */
	static ttstr getDisplayName(ttstr filename)
	{
		filename = TVPNormalizeStorageName(filename);
		TVPGetLocalName(filename);
		// if (filename != "") {
		// 	SHFILEINFO finfo;
		// 	if (SHGetFileInfo(filename.c_str(), 0, &finfo, sizeof finfo, SHGFI_DISPLAYNAME)) {
		// 		filename = finfo.szDisplayName;
		// 	} else {
		// 		TVPThrowExceptionMessage(TJS_W("SHGetFileInfo failed"));
		// 	}
		// }
		return filename;
	}

private:
	//	Žw’è‚ÌƒpƒX‚©‚çITEMIDLIST‚ðŽæ“¾
	// static ITEMIDLIST*	Path2ITEMIDLIST(const tjs_char* path)
	// {
	// 	IShellFolder* isf;
	// 	ITEMIDLIST* pidl;
	// 	if(SUCCEEDED(::SHGetDesktopFolder(&isf)))
	// 	{
	// 		ULONG	tmp;
	// 		if(SUCCEEDED(isf->ParseDisplayName(NULL, NULL, (LPOLESTR)path, &tmp, &pidl, &tmp)))
	// 			return pidl;
	// 	}
	// 	return NULL;
	// }

	//	ITEMIDLIST‚ð‰ð•ú
	// static void	FreeITEMIDLIST(ITEMIDLIST* pidl)
	// {
	// 	IMalloc*	im;
	// 	if(::SHGetMalloc(&im) != NOERROR)
	// 		im	= NULL;
	// 	if(im != NULL)
	// 		im->Free((void*)pidl);
	// }

	//	SHBrowserForFolder‚ÌƒR[ƒ‹ƒoƒbƒNŠÖ”
	// static int	CALLBACK SelectDirectoryCallBack(HWND hwnd, UINT msg, LPARAM lparam, LPARAM lpdata)
	// {
	// 	//	‰Šú‰»Žž
	// 	if(msg == BFFM_INITIALIZED)
	// 	{
	// 		//	‰ŠúƒtƒHƒ‹ƒ_‚ðŽw’è
	// 		ITEMIDLIST*	pidl;
	// 		pidl	= Path2ITEMIDLIST((tjs_char*)lpdata);
	// 		if(pidl != NULL)
	// 		{
	// 			::SendMessage(hwnd, BFFM_SETSELECTION, FALSE, (LPARAM)pidl);
	// 			FreeITEMIDLIST(pidl);
	// 		}

	// 		//	Å‘O–Ê‚ÖˆÚ“®
	// 		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	// 	}
	// 	return 0;
	// }

public:
	/**
	 * MD5ƒnƒbƒVƒ…’l‚ÌŽæ“¾
	 * @param filename ‘ÎÛƒtƒ@ƒCƒ‹–¼
	 * @return ƒnƒbƒVƒ…’li32•¶Žš‚Ì16i”ƒnƒbƒVƒ…•¶Žš—ñi¬•¶Žšjj
	 */
	static tjs_error TJS_INTF_METHOD getMD5HashString(tTJSVariant *result,
													  tjs_int numparams,
													  tTJSVariant **param,
													  iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		assert(false);//not implement;

		// ttstr filename = TVPGetPlacedPath(*param[0]);
		// IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
		// if (!in) TVPThrowExceptionMessage((ttstr(TJS_W("cannot open : ")) + param[0]->GetString()).c_str());

		// TVP_md5_state_t st;
		// TVP_md5_init(&st);

		// tjs_uint8 buffer[1024]; // > 16 digestƒoƒbƒtƒ@Œ“‚Ë‚é
		// DWORD size = 0;
		// while (in->Read(buffer, sizeof buffer, &size) == S_OK && size > 0) {
		// 	TVP_md5_append(&st, buffer, (int)size);
		// }
		// in->Release();

		// TVP_md5_finish(&st, buffer);

		// tjs_char ret[32+1], *hex = TJS_W("0123456789abcdef");
		// for (tjs_int i=0; i<16; i++) {
		// 	ret[i*2  ] = hex[(buffer[i] >> 4) & 0xF];
		// 	ret[i*2+1] = hex[(buffer[i]     ) & 0xF];
		// }
		// ret[32] = 0;
		// if (result) *result = ttstr(ret);
		return TJS_S_OK;
	}

	/**
	 * ƒpƒX‚ÌŒŸõ
	 * @param filename   ŒŸõ‘ÎÛƒtƒ@ƒCƒ‹–¼
	 * @param searchpath ŒŸõ‘ÎÛƒpƒXiƒ[ƒJƒ‹•\‹L(c:\`“™)‚Å";"‹æØ‚èCÈ—ªŽž‚ÍƒVƒXƒeƒ€‚ÌƒfƒtƒHƒ‹ƒgŒŸõƒpƒXj
	 * @return Œ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚ÍvoidCŒ©‚Â‚©‚Á‚½ê‡‚Íƒtƒ@ƒCƒ‹‚Ìƒtƒ‹ƒpƒX(file://./`)
	 */
	static tjs_error TJS_INTF_METHOD searchPath(tTJSVariant *result,
												tjs_int numparams,
												tTJSVariant **param,
												iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		// ttstr filename(*param[0]), searchpath;
		// if (TJS_PARAM_EXIST(1)) searchpath = *param[1];
		// WCHAR tmp[MAX_PATH+1];
		// DWORD len = ::SearchPathW(searchpath.length() ? searchpath.c_str() : NULL,
		// 						  filename.c_str(), NULL, MAX_PATH, tmp, NULL);
		// if (len > 0) {
		// 	// if (len > MAX_PATH) ...
		// 	tmp[MAX_PATH] = 0;
		// 	if (result) *result = TVPNormalizeStorageName(tmp);
		// } else {
		// 	// not found
		// 	if (result) result->Clear();
		// }
		return TJS_S_OK;
	}

	/*----------------------------------------------------------------------
	 * ƒJƒŒƒ“ƒgƒfƒBƒŒƒNƒgƒŠ
     ----------------------------------------------------------------------*/
	static ttstr getCurrentPath() {
		boost::filesystem::path p;
		boost::filesystem::current_path(p);
		ttstr result = p.c_str();
		// TCHAR crDir[MAX_PATH + 1];
		// GetCurrentDirectory(MAX_PATH + 1 , crDir);
		// ttstr result(crDir);
		return TVPNormalizeStorageName(result + L"/");
	}
	
	static void setCurrentPath(ttstr path) {
		if (!changeDirectory(path)) {
			ttstr mes;
			getLastError(mes);
			TVPThrowExceptionMessage(TJS_W("setCurrentPath failed:%1"), mes);
		}
	}
};

NCB_ATTACH_CLASS(StoragesFstat, Storages) {
	NCB_METHOD(clearStorageCaches);
	RawCallback("fstat",               &Class::fstat,               TJS_STATICMEMBER);
	RawCallback("getTime",             &Class::getTime,             TJS_STATICMEMBER);
	RawCallback("setTime",             &Class::setTime,             TJS_STATICMEMBER);
	NCB_METHOD(getLastModifiedFileTime);
	NCB_METHOD(setLastModifiedFileTime);
	NCB_METHOD(exportFile);
	NCB_METHOD(deleteFile);
	NCB_METHOD(truncateFile);
	NCB_METHOD(moveFile);
	NCB_METHOD(dirlist);
	NCB_METHOD(dirlistEx);
	NCB_METHOD(removeDirectory);
	NCB_METHOD(createDirectory);
	NCB_METHOD(createDirectoryNoNormalize);
	NCB_METHOD(changeDirectory);
	NCB_METHOD(setFileAttributes);
	NCB_METHOD(resetFileAttributes);
	NCB_METHOD(getFileAttributes);
	RawCallback("selectDirectory",     &Class::selectDirectory,     TJS_STATICMEMBER);
	NCB_METHOD(isExistentDirectory);
	NCB_METHOD(copyFile);
	NCB_METHOD(copyFileNoNormalize);
	NCB_METHOD(isExistentStorageNoSearchNoNormalize);
	NCB_METHOD(getDisplayName);
	RawCallback("getMD5HashString",    &Class::getMD5HashString,    TJS_STATICMEMBER);
	RawCallback("searchPath",          &Class::searchPath,          TJS_STATICMEMBER);
	Property("currentPath", &Class::getCurrentPath, &Class::setCurrentPath);
	Method(TJS_W("getTemporaryName"), &TVPGetTemporaryName);
};

// ƒeƒ“ƒ|ƒ‰ƒŠƒtƒ@ƒCƒ‹ˆ——pƒNƒ‰ƒX
class TemporaryFiles
{
public:
	TemporaryFiles() {};

	~TemporaryFiles() {
		std::vector<HFILE>::iterator it = handles.begin();
		while (it != handles.end()) {
			HFILE h = *it;
			::CloseHandle(h);
		}
	}

	bool entry(ttstr filename) {
		return _entry(filename);
	}

	bool entryFolder(ttstr filename) {
		return _entry(filename, true);
	}
	
private:
	std::vector<HFILE> handles;

	bool _entry(const ttstr &name, bool folder=false) {
		ttstr filename = TVPNormalizeStorageName(name);
		TVPGetLocalName(filename);
		if (filename.length()) {
			assert(false);//not implenment;
			// DWORD access = FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;
			// DWORD flag = folder ? FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_DELETE_ON_CLOSE : FILE_ATTRIBUTE_NORMAL|FILE_FLAG_DELETE_ON_CLOSE;
			// HFILE h = CreateFile(filename.c_str(),0,access,0,OPEN_EXISTING,flag,0);
			// if (h != INVALID_HANDLE_VALUE) {
			// 	handles.push_back(h);
			// 	return true;
			// }
		}
		return false;
	}
};

NCB_REGISTER_CLASS(TemporaryFiles) {
	Constructor();
	NCB_METHOD(entry);
	NCB_METHOD(entryFolder);
}

/**
 * “o˜^ˆ—Œã
 */
static void PostRegistCallback()
{
	tTJSVariant var;
	TVPExecuteExpression(TJS_W("Date"), &var);
	dateClass = var.AsObject();
	var.Clear();
	TVPExecuteExpression(TJS_W("Date.setTime"), &var);
	dateSetTime = var.AsObject();
	var.Clear();
	TVPExecuteExpression(TJS_W("Date.getTime"), &var);
	dateGetTime = var.AsObject();
	var.Clear();
	TVPExecuteExpression(StoragesFstatPreScript);
}

#define RELEASE(name) name->Release();name= NULL

/**
 * ŠJ•úˆ—‘O
 */
static void PreUnregistCallback()
{
	RELEASE(dateClass);
	RELEASE(dateSetTime);
}

NCB_POST_REGIST_CALLBACK(PostRegistCallback);
NCB_PRE_UNREGIST_CALLBACK(PreUnregistCallback);



//copy frome ncbind.cpp

// #include <windows.h>



// #define EXPORT(hr) extern "C" __declspec(dllexport) hr __stdcall
#define EXPORT(hr) extern "C" hr

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:V2Link=_V2Link@4")
#pragma comment(linker, "/EXPORT:V2Unlink=_V2Unlink@0")
#endif

//--------------------------------------
// int WINAPI
// DllEntryPoint(HINSTANCE /*hinst*/, unsigned long /*reason*/, void* /*lpReserved*/)
// {
// 	return 1;
// }

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;

EXPORT(HRESULT) fstat_V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	NCB_LOG_W("fstat_V2Link!!");

	// AutoRegisterで登録されたクラス等を登録する
	// ncbAutoRegister::AllRegist();

	// この時点での TVPPluginGlobalRefCount の値を
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// として控えておく。TVPPluginGlobalRefCount はこのプラグイン内で
	// 管理されている tTJSDispatch 派生オブジェクトの参照カウンタの総計で、
	// 解放時にはこれと同じか、これよりも少なくなってないとならない。
	// そうなってなければ、どこか別のところで関数などが参照されていて、
	// プラグインは解放できないと言うことになる。

	return S_OK;
}
//---------------------------------------------------------------------------
EXPORT(HRESULT) fstat_V2Unlink()
{
	// 吉里吉里側から、プラグインを解放しようとするときに呼ばれる関数

	// もし何らかの条件でプラグインを解放できない場合は
	// この時点で E_FAIL を返すようにする。
	// ここでは、TVPPluginGlobalRefCount が GlobalRefCountAtInit よりも
	// 大きくなっていれば失敗ということにする。
	if (TVPPluginGlobalRefCount > GlobalRefCountAtInit) {
		NCB_LOG_W("V2Unlink ...failed");
		return E_FAIL;
		// E_FAIL が帰ると、Plugins.unlink メソッドは偽を返す
	} else {
		NCB_LOG_W("V2Unlink");
	}
	/*
		ただし、クラスの場合、厳密に「オブジェクトが使用中である」ということを
		知るすべがありません。基本的には、Plugins.unlink によるプラグインの解放は
		危険であると考えてください (いったん Plugins.link でリンクしたら、最後ま
		でプラグインを解放せず、プログラム終了と同時に自動的に解放させるのが吉)。
	*/

	// AutoRegisterで登録されたクラス等を削除する
	ncbAutoRegister::AllUnregist();

	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	return S_OK;
}


//---------------------------------------------------------------------------
// static変数の実体

// auto register 先頭ポインタ
// ncbAutoRegister::ThisClassT const*
// ncbAutoRegister::_top[ncbAutoRegister::LINE_COUNT] = NCB_INNER_AUTOREGISTER_LINES_INSTANCE;

