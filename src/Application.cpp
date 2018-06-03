#include "Application.h"
#include "tjsCommHead.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include "GLDevice.hpp"
#include "DebugIntf.h"
#include "WindowImpl.h"

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
bool StartApplication( int argc, tjs_char* argv[] )
{
	wprintf(L"StartApplication!\n");
}

void tTVPApplication::PrintConsole( const wchar_t* mes, unsigned long len, bool iserror)
{
	// wprintf(mes);
	// std::wstring s(mes,len);
	wprintf(TJS_W("%ls\n"),mes);
	// std::cout<<s;
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
static void render_update(unsigned int interval)
{
	// TVPAddLog(TJS_W("render_update!!!"));
	unsigned int cur_tick = time_now();
	if(cur_tick - last_tick > Event_interval)
	{
		TVPProcessContinuousHandlerEventFlag = true;
		last_tick = cur_tick;
	}
	tTJSNI_BaseLayer* lay = TVPMainWindow->GetDrawDevice()->GetPrimaryLayer();
	// ttstr text("");
	// wprintf(TJS_W("%d,%.1f\n"),interval,1000.0/interval);
	// text.printf(TJS_W("fps:%d"),1000/interval);
	// TVPAddLog(text);
	// wprintf(text.c_str());
	// wprintf(TJS_W("\n"));
	// lay->DrawText(lay->GetWidth()-100,0,
	// text,0xffffff,
	// 0xff,true,0,0,0,0,0);


	TVPDeliverAllEvents();
	TVPMainWindow->UpdateContent();


}
void tTVPApplication::Run()
{
	// while(sdl_loop());
	regist_update(render_update);
	// if(TVPMainWindow)
	// 	TVPMainWindow->UpdateContent();
	sdl_loop();
}
