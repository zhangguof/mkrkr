#ifndef _SDL_EVENT_H_
#define _SDL_EVENT_H_
#include "SDL.h"

#define TIMER_EVNET_CODE (0x1000)

typedef void (*tUseEventCb)(void* param);

struct TimerEvent:public SDL_UserEvent
{
	TimerEvent(void* func,void* data);
	SDL_UserEvent& get_event();
};



#endif