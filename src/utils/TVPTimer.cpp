
#include "tjsCommHead.h"
#include "TVPTimer.h"
#include "SDL.h"
#include "sdlEvent.hpp"
// #include "WindowsUtil.h"

// SDL_TimerID SDL_AddTimer(Uint32            interval,
//                          SDL_TimerCallback callback,
//                          void*             param)

// SDL_bool SDL_RemoveTimer(SDL_TimerID id)


TVPTimer::TVPTimer() : event_(NULL), interval_(1000), enabled_(true) {
	// CreateUtilWindow();
	tid = 0;
}

TVPTimer::~TVPTimer() {
	Destroy();
	if( event_ ) {
		delete event_;
	}
}

void TVPTimer::Destroy() {
	if(tid != 0)
	{
		SDL_RemoveTimer(tid);
	}
}
void TVPTimer::WndProc() {
	if(tid != 0 && enabled_)
	{
		this->FireEvent();
	}
}
void TVPTimer::UpdateTimer() {
	if(tid != 0)
	{
		if(SDL_RemoveTimer(tid))
		{
			tid = 0;
		}
	}

	if( interval_ > 0 && enabled_ && event_ != NULL ) {
		// if( ::SetTimer( window_handle_, 1, interval_, NULL ) == 0 ) {
		// 	TVPThrowWindowsErrorException();
		// }
		SDL_TimerID new_id = SDL_AddTimer(interval_,
		TVPTimer::timer_cb,this);
		if(new_id == 0)
		{
			// printf("")
			SDL_Log("SDL_AddTimer error:%s", SDL_GetError());
			return;
		}
		tid = new_id;
	}
}

void event_timer_cb(void* param)
{
	TVPTimer* _this = static_cast<TVPTimer*>(param);
	_this->WndProc();
}

uint32_t TVPTimer::timer_cb(uint32_t interval, void* param)
{
	// TVPTimer* _this = static_cast<TVPTimer*>(param);
	SDL_Event event;
	TimerEvent te((void*)event_timer_cb, param);

	event.type = SDL_USEREVENT;
    event.user = te.get_event();

    SDL_PushEvent(&event);

	//process in main sdl loop!
	// _this->WndProc();
	return interval;
}

