#include "sdlEvent.hpp"
#include "SDL.h"

TimerEvent::TimerEvent(void* func,void* data)
{
	data1 = func;
	data2 = data;
	code = TIMER_EVNET_CODE;
	type = SDL_USEREVENT;
}
SDL_UserEvent& TimerEvent::get_event()
{
	// return *(static_cast<SDL_UserEvent*>(this));
	return *this;
}

void process_use_envet(SDL_UserEvent& e)
{
	// SDL_Log("process_use_envet:%d",e.code);

	if(e.code == TIMER_EVNET_CODE)
	{
		tUseEventCb cb =  (tUseEventCb)(e.data1); //static_cast<tUseEventCb>(e.data1);
		cb(e.data2);
	}
}