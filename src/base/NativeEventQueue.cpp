
#include "tjsCommHead.h"
#include "NativeEventQueue.h"


void NativeEventQueueImplement::WndProc(NativeEvent& event) {
//in main thread.

	NativeEventQueueIntarface *_this = reinterpret_cast<NativeEventQueueIntarface*>(event.data1);
	if( _this != NULL ) {
		_this->Dispatch( event );
	}
}

// デフォルトハンドラ
void NativeEventQueueImplement::HandlerDefault( NativeEvent& event ) {
	SDL_Log("handle HandlerDefault???");
	// event.Result = ::DefWindowProc(event.HWnd,event.Message,event.WParam,event.LParam);
}
void NativeEventQueueImplement::Allocate() {
	// CreateUtilWindow();
}
void NativeEventQueueImplement::Deallocate() {
	// if( window_handle_ != NULL ) {
	// 	::SetWindowLongPtr( window_handle_, GWLP_USERDATA, (LONG_PTR)NULL );
	// 	::DestroyWindow( window_handle_ );
	// 	window_handle_ = NULL;
	// }
}
void process_native_event(void* p_imp, SDL_UserEvent& e)
{
		NativeEventQueueImplement* _this = reinterpret_cast<NativeEventQueueImplement*>(p_imp);
		NativeEvent& event = static_cast<NativeEvent&>(e);
		_this->WndProc(event);
}

void NativeEventQueueImplement::PostEvent(NativeEvent e ) {
	// ::PostMessage( window_handle_, event.Message, event.WParam, event.LParam );
	SDL_Event event;
	if(e.data1 == nullptr)
	{
		e.data1 =  reinterpret_cast<void*>(this);
	}
	event.type = SDL_USEREVENT;
	event.user = e.get_event();
	SDL_PushEvent(&event);
}

