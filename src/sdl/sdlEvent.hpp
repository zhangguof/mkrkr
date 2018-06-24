#ifndef _SDL_EVENT_H_
#define _SDL_EVENT_H_
#include "SDL.h"

#define TIMER_EVNET_CODE (0x1000)

#define Native_Event_Code_MASK (0x10000)
#define Native_Event_Code (Native_Event_Code_MASK + 1)





typedef void (*tUseEventCb)(void* param);

//SDL_UserEvent:
// Uint32	type	value obtained from SDL_RegisterEvents()
// Uint32	timestamp	timestamp of the event
// Uint32	windowID	the associated window, if any
// Sint32	code	user defined event code
// void*	data1	user defined data pointer
// void*	data2	user defined data pointer

struct TimerEvent:public SDL_UserEvent
{
	TimerEvent(void* func,void* data);
	SDL_UserEvent& get_event();
};



#endif