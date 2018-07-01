#ifndef __T_APPLICATION_H__
#define __T_APPLICATION_H__
#include <string>
#include "tjs.h"

class SDLWindow;

ttstr ExePath();


class tTVPApplication{
	std::wstring title_;
	std::vector<class SDLWindow*> windows_list_;
	bool tarminate_;
public:
	
	tTVPApplication();
	~tTVPApplication();
	bool StartApplication( int argc, char* argv[] );

	void PrintConsole( const wchar_t* mes, unsigned long len, bool iserror = false );
    void PrintConsole(const ttstr mes,bool iserror=false);
	std::wstring GetTitle() const { return title_; }
	void SetTitle( const std::wstring& caption );

	void AddWindow(SDLWindow* win ) {
		windows_list_.push_back( win );
	}
	void RemoveWindow(SDLWindow* win );
	void Run();

	// void LoadImageRequest( class iTJSDispatch2 *owner, class tTJSNI_Bitmap* bmp, const ttstr &name );

};
extern tTVPApplication* Application;

#endif
