#include "Application.h"
#include "tjsCommHead.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include "GLDevice.hpp"
#include "DebugIntf.h"
#include "WindowImpl.h"
#include "FontSystem.h"

//absolute path,"/" at last.
ttstr ExePath() {
	auto cur_path = boost::filesystem::current_path();
	ttstr p(cur_path.c_str());
	if(p.GetLastChar()!=TJS_W('/'))
		p = p + TJS_W('/');

	return p;
}

tTVPApplication* Application;

tTVPApplication::tTVPApplication(){
	tarminate_ = false;
}
tTVPApplication::~tTVPApplication(){}

const int max_arg_cnt = 20;
int _argc;
// tjs_char ** _wargv;
ttstr _argv[max_arg_cnt];

const tjs_char* sel_data_path = NULL;
bool tTVPApplication::StartApplication( int argc, char* argv[] )
{
	wprintf(L"StartApplication!\n");
    SDL_Log("Start Application!!!!");
	
	_argc = argc;
	for(int i= 0;i<argc;++i)
	{
		_argv[i] = argv[i];
		// TVPAddLog(_argv[i]);
	}
	for(int i=0;i<argc;++i)
	{
		if(_argv[i] == TJS_W("-data") && i+1<argc)
		{
			sel_data_path = _argv[i+1].c_str();
			wprintf(TJS_W("====sel data path:%ls\n"),sel_data_path);
			break;
		}
	}

}

void tTVPApplication::PrintConsole( const wchar_t* mes, unsigned long len, bool iserror)
{
	// wprintf(mes);
	// std::wstring s(mes,len);
//    printf("%ls\n"),mes);
	// std::cout<<s;
    fprintf(stderr, "%ls\n",mes);
}

void tTVPApplication::PrintConsole(const ttstr mes, bool iserror)
{
    fprintf(stderr, "%s\n",mes.AsNarrowStdString().c_str());
}


void tTVPApplication::RemoveWindow( SDLWindow* win ) {
	std::vector<class SDLWindow*>::iterator it = std::remove( windows_list_.begin(), windows_list_.end(), win );
	if( it != windows_list_.end() ) {
		windows_list_.erase( it, windows_list_.end() );
	}
}

void tTVPApplication::SetTitle( const std::wstring& caption ) {
	title_ = caption;
}

// onLoaded( dic, is_async, is_error, error_mes ); エラーは
// sync ( main thead )
// void LoadRequest( iTJSDispatch2 *owner, tTJSNI_Bitmap* bmp, const ttstr &name ) {
// 	//tTVPBaseBitmap* dest = new tTVPBaseBitmap( 32, 32, 32 );
// 	tTVPBaseBitmap dest( TVPGetInitialBitmap() );
// 	iTJSDispatch2* metainfo = NULL;
// 	ttstr nname = TVPNormalizeStorageName(name);
// 	if( TVPCheckImageCache(nname,&dest,glmNormal,0,0,TVP_clNone,&metainfo) ) {
// 		// キャッシュ内に発見、即座に読込みを完了する
// 		bmp->CopyFrom( &dest );
// 		bmp->SetLoading( false );

// 		tTJSVariant param[4];
// 		param[0] = tTJSVariant(metainfo,metainfo);
// 		if( metainfo ) metainfo->Release();
// 		param[1] = 0; // false
// 		param[2] = 0; // false
// 		param[3] = TJS_W(""); // error_mes
// 		static ttstr eventname(TJS_W("onLoaded"));
// 		TVPPostEvent(owner, owner, eventname, 0, TVP_EPT_IMMEDIATE, 4, param);
// 		return;
// 	}
// 	if( TVPIsExistentStorage(name) == false ) {
// 		TVPThrowExceptionMessage(TVPCannotFindStorage, name);
// 	}
// 	ttstr ext = TVPExtractStorageExt(name);
// 	if(ext == TJS_W("")) {
// 		TVPThrowExceptionMessage(TJS_W("Filename extension not found/%1"), name);
// 	}

// 	PushLoadQueue( owner, bmp, nname );
// }

// tTJSCriticalSectionHolder cs_holder(TVPCreateStreamCS);
//	tTJSBinaryStream* stream = TVPCreateStream(nname, TJS_BS_READ);
// TVPCreateStream はロックされているので、非同期で実行可能
// void PushLoadQueue( iTJSDispatch2 *owner, tTJSNI_Bitmap *bmp, const ttstr &nname ) {
// 	tTVPImageLoadCommand* cmd = new tTVPImageLoadCommand();
// 	cmd->owner_ = owner;
// 	owner->AddRef();
// 	cmd->bmp_ = bmp;
// 	cmd->path_ = nname;
// 	cmd->dest_ = new tTVPTmpBitmapImage();
// 	cmd->result_.Clear();
// 	{
// 		// キューをロックしてプッシュ
// 		tTJSCriticalSectionHolder cs(CommandQueueCS);
// 		CommandQueue.push(cmd);
// 	}
// 	// 追加したことをイベントで通知
// 	PushCommandQueueEvent.Set();
// }

// void tTVPApplication::LoadImageRequest( class iTJSDispatch2 *owner, class tTJSNI_Bitmap* bmp, const ttstr &name ) {
// 		LoadRequest( owner, bmp, name );
// }

// void tTVPApplication::RemoveWindow( TTVPWindowForm* win ) {
// 	std::vector<class TTVPWindowForm*>::iterator it = std::remove( windows_list_.begin(), windows_list_.end(), win );
// 	if( it != windows_list_.end() ) {
// 		windows_list_.erase( it, windows_list_.end() );
// 	}
// }
extern tTJSNI_Window * TVPMainWindow;
extern void TVPDeliverAllEvents(); // called from (indirectly) the OS
extern bool TVPProcessContinuousHandlerEventFlag;
unsigned int Event_interval = 1000/60; // 1/60.0 *1000 ms

static unsigned int last_tick = 0;
static unsigned int fps_tick = 0;
static tTJSNI_BaseLayer* gfps_lay = NULL;
static ttstr fps_text("");
void init_fps()
{
	if(gfps_lay) return;
	if(!TVPMainWindow) return;
	tTJSNI_BaseLayer* main_lay = TVPMainWindow->GetDrawDevice()->GetPrimaryLayer();
	tTVPLayerManager* lay_m = main_lay->GetManager();
	auto lays = lay_m->GetAllNodes();
	
	for(auto i:lays)
	{
		if(i->GetName() == TJS_W("fps"))
		{
			gfps_lay = i;
			break;
		}
	}

}

extern void timer_update();

void event_update()
{
	timer_update();

}


static void update(unsigned int interval)
{
	// TVPAddLog(TJS_W("render_update!!!"));
	unsigned int cur_tick = time_now();
	
	if(cur_tick - last_tick >= Event_interval)
	{
		TVPProcessContinuousHandlerEventFlag = true;
		last_tick = cur_tick;
	}
	if(gfps_lay)
	{
		
		fps_text.printf(TJS_W("fps:%.1f"),1000.0/(cur_tick - fps_tick));
		gfps_lay->FillRect(tTVPRect(0,0,
				gfps_lay->GetWidth(),
				gfps_lay->GetHeight()),
		0x00000000);
		gfps_lay->DrawText(0,0,fps_text,0x0000ff,
		0xff,true,0,0,0,0,0);	
	}
	event_update();


	TVPDeliverAllEvents();
	//render update
	if(TVPMainWindow)
		TVPMainWindow->UpdateContent();
	// wprintf(TJS_W("=====:%d\n"),interval);
	fps_tick = cur_tick;


}

// extern void TVPInializeFontRasterizers();
// extern FontSystem* TVPFontSystem;

// void pre_run()
// {
// 	TVPInializeFontRasterizers();
// 	TVPFontSystem->AddFont(TJS_W("font/NotoSansCJKsc-Black.otf"));
// }
extern void al_loop(uint32_t interval);
extern void VideoPlayer_Loop(uint32_t interval);

void tTVPApplication::Run()
{
	// while(sdl_loop());
	// pre_run();
	init_fps();
	regist_update(al_loop);
	regist_update(update);
	regist_update(VideoPlayer_Loop);
	
	// if(TVPMainWindow)
	// 	TVPMainWindow->UpdateContent();
	sdl_loop();
}
