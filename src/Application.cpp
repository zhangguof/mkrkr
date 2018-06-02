#include "Application.h"
#include "tjsCommHead.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include "GLDevice.hpp"
#include "DebugIntf.h"
// #include "WindowImpl.h"

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
// extern tTJSNI_Window * TVPMainWindow;
static void render_update(unsigned int interval)
{
	// TVPAddLog(TJS_W("render_update!!!"));

}
void tTVPApplication::Run()
{
	// while(sdl_loop());
	regist_update(render_update);
	// if(TVPMainWindow)
	// 	TVPMainWindow->UpdateContent();
	sdl_loop();
}
