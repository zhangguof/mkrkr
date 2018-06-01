#ifndef __T_APPLICATION_H__
#define __T_APPLICATION_H__
#include <string>
#include "tjs.h"

ttstr ExePath();


class tTVPApplication{
	std::wstring title_;
	std::vector<class TTVPWindowForm*> windows_list_;
	bool tarminate_;
public:
	
	tTVPApplication();
	~tTVPApplication();
	bool StartApplication( int argc, tjs_char* argv[] );

	void PrintConsole( const wchar_t* mes, unsigned long len, bool iserror = false );
	std::wstring GetTitle() const { return title_; }
	void SetTitle( const std::wstring& caption );

	// void AddWindow( class TTVPWindowForm* win ) {
	// 	windows_list_.push_back( win );
	// }
	// void RemoveWindow( class TTVPWindowForm* win );
	void Run();

};
extern tTVPApplication* Application;

#endif