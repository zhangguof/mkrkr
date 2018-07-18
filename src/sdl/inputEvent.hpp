//
//  inputEvent.hpp
//  mkrkr
//
//  Created by tony on 2018/7/19.
//

#ifndef inputEvent_hpp
#define inputEvent_hpp

#include <stdio.h>
#include "SDL.h"
#include "tjsCommHead.h"
#include "WindowImpl.h"
#include <set>
#include <map>
typedef void (*input_event_hook)(void* data,SDL_Event& e);
typedef std::vector<input_event_hook> tEvent_array;
typedef std::shared_ptr<tEvent_array> tPtrEvent_arrays;
extern  std::set<uint32_t> input_events;

extern void process_input_event(SDL_Event& e,SDLWindow* win);
//extern std::map<uint32_t,tPtrEvent_arrays> input_event_handles;

//extern void regist_input_event(uint32_t t,input_event_hook hook);




#endif /* inputEvent_hpp */

