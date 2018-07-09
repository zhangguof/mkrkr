#include "ncbind/ncbind.hpp"
#include <vector>
#include "win_def.h"
#include "tp_stub.h"
using namespace std;
// #include <process.h>
// struct tTVPWindowMessage
// {
// 	unsigned int Msg; // window message
// 	WPARAM WParam;  // WPARAM
// 	LPARAM LParam;  // LPARAM
// 	LRESULT Result;  // result
// };

#define WM_SAVE_TLG_PROGRESS (0x1000+4)
#define WM_SAVE_TLG_DONE     (0x1000+5)

iTJSDispatch2 *getLayerClass(void)
{
  tTJSVariant var;
  TVPExecuteExpression(TJS_W("Layer"), &var);
  return  var.AsObjectNoAddRef();
}

iTJSDispatch2 *getLayerAssignImages(void)
{
  tTJSVariant var;
  TVPExecuteExpression(TJS_W("Layer.assignImages"), &var);
  return  var.AsObjectNoAddRef();
}

#include "compress.hpp"
#include "savetlg5.hpp"
#include "savepng.hpp"

//---------------------------------------------------------------------------
// ウインドウ拡張
//---------------------------------------------------------------------------

class WindowSaveImage;

/**
 * セーブ処理スレッド用情報
 */
class SaveInfo {

	friend class WindowSaveImage;
	
protected:

	// 初期化変数
	WindowSaveImage *notify; //< 情報通知先
	tTJSVariant layer; //< レイヤ
	tTJSVariant filename; //< ファイル名
	tTJSVariant info;  //< 保存用タグ情報
	bool canceled;        //< キャンセル指示
	tTJSVariant handler;  //< ハンドラ値
	tTJSVariant progressPercent; //< 進行度合い
	
protected:
	/**
	 * 現在の状態の通知
	 * @param percent パーセント
	 */
	bool progress(int percent);

	/**
	 * 呼び出し用
	 */
	static bool progressFunc(int percent, void *userData) {
		SaveInfo *self = (SaveInfo*)userData;
		return self->progress(percent);
	}
	
	// 経過イベント送信
	void eventProgress(iTJSDispatch2 *objthis) {
		tTJSVariant *vars[] = {&handler, &progressPercent, &layer, &filename};
		objthis->FuncCall(0, L"onSaveLayerImageProgress", NULL, NULL, 4, vars, objthis);
	}

	// 終了イベント送信
	void eventDone(iTJSDispatch2 *objthis) {
		tTJSVariant result = canceled ? 1 : 0;
		tTJSVariant *vars[] = {&handler, &result, &layer, &filename};
		objthis->FuncCall(0, L"onSaveLayerImageDone", NULL, NULL, 4, vars, objthis);
	}
	
public:
	// コンストラクタ
	SaveInfo(int handler, WindowSaveImage *notify, tTJSVariant layer, const tjs_char *filename, tTJSVariant info)
		: handler(handler), notify(notify), layer(layer), filename(filename), info(info), canceled(false) {}
	
	// デストラクタ
	~SaveInfo() {}

	// ハンドラ取得
	int getHandler() {
		return (int)handler;
	}
	
 	// 処理開始
	void start();

	// 処理キャンセル
	void cancel() {
		canceled = true;
	}

	// 強制終了
	void stop() {
		canceled = true;
		notify = NULL;
	}
};

/**
 * ウインドウにレイヤセーブ機能を拡張
 */
class WindowSaveImage {

public:
	iTJSDispatch2 *objthis; //< オブジェクト情報の参照

	vector<SaveInfo*> saveinfos; //< セーブ中情報保持用

	// 実行スレッド
	static void checkThread(void *data) {
		((SaveInfo*)data)->start();
	}

	// 経過通知
	void eventProgress(SaveInfo *sender) {
		int handler = sender->getHandler();
		if (saveinfos[handler] == sender) {
			sender->eventProgress(objthis);
		}
	}

	// 終了通知
	void eventDone(SaveInfo *sender) {
		int handler = sender->getHandler();
		if (saveinfos[handler] == sender) {
			saveinfos[handler] = NULL;
			sender->eventDone(objthis);
		}
		delete sender;
	}

	/*
	 * ウインドウイベント処理レシーバ
	 */
	// static bool __stdcall receiver(void *userdata, tTVPWindowMessage *Message) {
	// 	if (Message->Msg == WM_SAVE_TLG_PROGRESS) {
	// 		iTJSDispatch2 *obj = (iTJSDispatch2*)userdata;
	// 		WindowSaveImage *self = ncbInstanceAdaptor<WindowSaveImage>::GetNativeInstance(obj);
	// 		if (self) {
	// 			self->eventProgress((SaveInfo*)Message->WParam);
	// 		}
	// 		return true;
	// 	} else if (Message->Msg == WM_SAVE_TLG_DONE) {
	// 		iTJSDispatch2 *obj = (iTJSDispatch2*)userdata;
	// 		WindowSaveImage *self = ncbInstanceAdaptor<WindowSaveImage>::GetNativeInstance(obj);
	// 		if (self) {
	// 			self->eventDone((SaveInfo*)Message->WParam);
	// 		}
	// 		return true;
	// 	}
	// 	return false;
	// }

	// ユーザメッセージレシーバの登録/解除
	// void setReceiver(tTVPWindowMessageReceiver receiver, bool enable) {
	// 	tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
	// 	tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
	// 	tTJSVariant userdata = (tTVInteger)(tjs_int)objthis;
	// 	tTJSVariant *p[] = {&mode, &proc, &userdata};
	// 	if (objthis->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, objthis) != TJS_S_OK) {
	// 		TVPThrowExceptionMessage(L"can't regist user message receiver");
	// 	}
	// }

public:

	/**
	 * コンストラクタ
	 */
	WindowSaveImage(iTJSDispatch2 *objthis) : objthis(objthis) {
		// setReceiver(receiver, true);
		printf("init WindowSaveImage.....\n");
	}

	/**
	 * デストラクタ
	 */
	~WindowSaveImage() {
		// setReceiver(receiver, false);
		for (int i=0;i<(int)saveinfos.size();i++) {
			SaveInfo *saveinfo = saveinfos[i];
			if (saveinfo) {
				saveinfo->stop();
				saveinfos[i] = NULL;
			}
		}
	}

	/**
	 * メッセージ送信
	 * @param msg メッセージ
	 * @param wparam WPARAM
	 * @param lparam LPARAM
	 */
	void postMessage(UINT msg, WPARAM wparam=NULL, LPARAM lparam=NULL) {
		// ウィンドウハンドルを取得して通知
		tTJSVariant val;
		objthis->PropGet(0, TJS_W("HWND"), NULL, &val, objthis);
		// HWND hwnd = reinterpret_cast<HWND>((tjs_int)(val));
		// ::PostMessage(hwnd, msg, wparam, lparam);
	}

	/**
	 * レイヤセーブ開始
	 * @param layer レイヤ
	 * @param filename ファイル名
	 * @param info タグ情報
	 */
	int startSaveLayerImage(tTJSVariant layer, const tjs_char *filename, tTJSVariant info) {
		int handler = saveinfos.size();
		for (int i=0;i<(int)saveinfos.size();i++) {
			if (saveinfos[i] == NULL) {
				handler = i;
				break;
			}
		}
		if (handler >= (int)saveinfos.size()) {
			saveinfos.resize(handler + 1);
		}

		// 保存用にレイヤを複製する
		tTJSVariant newLayer;
		{
			// 新しいレイヤを生成
			tTJSVariant window(objthis, objthis);
			tTJSVariant primaryLayer;
			objthis->PropGet(0, L"primaryLayer", NULL, &primaryLayer, objthis);
			tTJSVariant *vars[] = {&window, &primaryLayer};
			iTJSDispatch2 *obj;
			if (TJS_SUCCEEDED(getLayerClass()->CreateNew(0, NULL, NULL, &obj, 2, vars, objthis))) {

				// 名前づけ
				tTJSVariant name = "saveLayer:";
				name +=filename;
				obj->PropSet(0, L"name", NULL, &name, obj);

				// 元レイヤの画像を複製
				tTJSVariant *param[] = {&layer};
				if (TJS_SUCCEEDED(getLayerAssignImages()->FuncCall(0, NULL, NULL, NULL, 1, param, obj))) {
					newLayer = tTJSVariant(obj, obj);
					obj->Release();
				} else {
					obj->Release();
					TVPThrowExceptionMessage(L"保存処理用レイヤへの画像の複製に失敗しました");
				}
			} else {
				TVPThrowExceptionMessage(L"保存処理用レイヤの生成に失敗しました");
			}
		}
		SaveInfo *saveInfo = new SaveInfo(handler, this, newLayer, filename, info);
		saveinfos[handler] = saveInfo;
		// _beginthread(checkThread, 0, saveInfo);
		saveInfo->start();
		return handler;
	}
	
	/**
	 * レイヤセーブのキャンセル
	 */
	void cancelSaveLayerImage(int handler) {
		if (handler < (int)saveinfos.size() && saveinfos[handler] != NULL) {
			saveinfos[handler]->cancel();
		}
	}

	/**
	 * レイヤセーブの中止
	 */
	void stopSaveLayerImage(int handler) {
		if (handler < (int)saveinfos.size() && saveinfos[handler] != NULL) {
			saveinfos[handler]->stop();
			saveinfos[handler] = NULL;
		}
	}
};


/**
 * 現在の状態の通知
 * @param percent パーセント
 */
bool
SaveInfo::progress(int percent)
{
	if ((int)progressPercent != percent) {
		progressPercent = percent;
		// if (notify) {
		// 	notify->postMessage(WM_SAVE_TLG_PROGRESS, (WPARAM)this);
		// 	Sleep(0);
		// }
		printf("SaveInfo:progress==:%d\n",percent);
	}
	return canceled;
}

/*
 * 保存処理開始
 */
void
SaveInfo::start()
{
	iTJSDispatch2  *lay = layer.AsObjectNoAddRef();
	iTJSDispatch2  *nfo = info.Type() == tvtObject ? info.AsObjectNoAddRef() : NULL;
	const tjs_char *fn  = filename.GetString();
	ttstr ext(TVPExtractStorageExt(ttstr(fn)));
	ext.ToLowerCase();
	// 画像をセーブ（拡張子別）
	if (ext == TJS_W(".png")) {
		CompressAndSave<CompressPNG >::saveLayerImage(lay, fn, nfo, progressFunc, (void*)this);
	} else {
		assert(false);
		// CompressAndSave<CompressTLG5>::saveLayerImage(lay, fn, nfo, progressFunc, (void*)this);
	}
	// 完了通知
	if (notify) {
		// notify->postMessage(WM_SAVE_TLG_DONE, (WPARAM)this);
		// Sleep(0);
		notify->eventDone(this);
		printf("SaveInfo eventDone!!\n");

	} else {
		delete this;
	}
}

//---------------------------------------------------------------------------

// インスタンスゲッタ
NCB_GET_INSTANCE_HOOK(WindowSaveImage)
{
	NCB_INSTANCE_GETTER(objthis) { // objthis を iTJSDispatch2* 型の引数とする
		ClassT* obj = GetNativeInstance(objthis);	// ネイティブインスタンスポインタ取得
		if (!obj) {
			obj = new ClassT(objthis);				// ない場合は生成する
			SetNativeInstance(objthis, obj);		// objthis に obj をネイティブインスタンスとして登録する
		}
		return obj;
	}
};

NCB_ATTACH_CLASS_WITH_HOOK(WindowSaveImage, Window) {
	NCB_METHOD(startSaveLayerImage);
	NCB_METHOD(cancelSaveLayerImage);
	NCB_METHOD(stopSaveLayerImage);
};

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

EXPORT(HRESULT) layerExSave_V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	NCB_LOG_W("layerExSave_V2Link");

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
EXPORT(HRESULT) layerExSave_V2Unlink()
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
	// ncbAutoRegister::AllUnregist();

	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	return S_OK;
}


//---------------------------------------------------------------------------
// static変数の実体

// auto register 先頭ポインタ
// ncbAutoRegister::ThisClassT const*
// ncbAutoRegister::_top[ncbAutoRegister::LINE_COUNT] = NCB_INNER_AUTOREGISTER_LINES_INSTANCE;



