
#include "tjsCommHead.h"
#include "NativeEventQueue.h"


void NativeEventQueueImplement::WndProc(NativeEvent& event) {
//in main thread.

	NativeEventQueueImplement *_this = reinterpret_cast<NativeEventQueueImplement*>(event.get_data1());
	if( _this != NULL ) {
		_this->Dispatch( event );
		_this->PopEvent(event._id);
	}
}

// デフォルトハンドラ
void NativeEventQueueImplement::HandlerDefault( NativeEvent& event ) {
	// SDL_Log("handle HandlerDefault???:event:%d",event.get_code());
	// event.Result = ::DefWindowProc(event.HWnd,event.Message,event.WParam,event.LParam);
}
void NativeEventQueueImplement::Allocate() {
	// CreateUtilWindow();
	ev_map.clear();
}
void NativeEventQueueImplement::Deallocate() {
	// if( window_handle_ != NULL ) {
	// 	::SetWindowLongPtr( window_handle_, GWLP_USERDATA, (LONG_PTR)NULL );
	// 	::DestroyWindow( window_handle_ );
	// 	window_handle_ = NULL;
	// }
	ev_map.clear();
}
void process_native_event(void* p_imp, SDL_UserEvent& e)
{
		NativeEventQueueImplement* _this = reinterpret_cast<NativeEventQueueImplement*>(p_imp);
		int64_t eid = reinterpret_cast<int64_t>(e.data2);
		auto pevent = _this->find_event(eid);
		if(pevent)
			_this->WndProc(*pevent);
}

void NativeEventQueueImplement::PostEvent(NativeEvent e) {
	// ::PostMessage( window_handle_, event.Message, event.WParam, event.LParam );
	
	auto pe = std::make_shared<NativeEvent>(e);
	ev_map[pe->_id] = pe;
	pe->set_data1(reinterpret_cast<void*>(this));
	// if(!e.get_data1())
	// {
	// 	e.set_data1(reinterpret_cast<void*>(this));
	// }
	// if(e.data1 == nullptr)
	// {
	// 	e.data1 =  reinterpret_cast<void*>(this);
	// }
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user = pe->get_event();

	SDL_PushEvent(&event);
}

void NativeEventQueueImplement::PopEvent(int64_t eid)
{
	//after process event.
	if(eid!=-1)
	{
		auto find_it = ev_map.find(eid);
		if(find_it!=ev_map.end())
		{
			ev_map.erase(find_it);
		}
	}
}
tPtrNativeEvent NativeEventQueueImplement::find_event(int64_t eid)
{
	auto find_it = ev_map.find(eid);
	if(find_it != ev_map.end())
	{
		return find_it->second;
	}
	return nullptr;
}