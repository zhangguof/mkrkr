//
//  inputEvent.cpp
//  mkrkr
//
//  Created by tony on 2018/7/19.
//

#include "inputEvent.hpp"
#include "win_def.h"



//std::map<uint32_t,tPtrEvent_arrays> input_event_handles;
//void regist_input_event(uint32_t t,input_event_hook hook)
//{
//    auto find_it = input_event_handles.find(t);
//    if(find_it == input_event_handles.end())
//    {
//        input_event_handles[t] = std::make_shared<tEvent_array>();
//    }
//    auto p = input_event_handles[t];
//    p->push_back(hook);
//}



//simula right click in ios.
static long long finger1 = 0;
static long long finger2 = 0;
static int finger_cnt = 0;
static bool right_click_down = false;

bool handle_finger(SDL_TouchFingerEvent& e)
{
    //handle 2 finger
    if(e.type == SDL_FINGERDOWN)
    {
        if(finger1 && finger2)
            return false;
        if(!finger1){
            finger1 = e.fingerId;
            finger_cnt++;
            
        }
        else if(!finger2)
        {
            finger2 = e.fingerId;
            finger_cnt++;
        }
        if(finger_cnt==2)
        {
            right_click_down = true;
            //            printf("handle right click down!:%f,%f",e.x,e.y);
            return true;
        }
    }
    else if(e.type == SDL_FINGERUP)
    {
        if(finger1!=e.fingerId && finger2!=e.fingerId)
            return false;
        if(finger1 == e.fingerId)
        {
            finger1 = 0;
            finger_cnt--;
        }
        else if(finger2 == e.fingerId)
        {
            finger2 = 0;
            finger_cnt--;
        }
        if(right_click_down && finger_cnt==0)
        {
            right_click_down = false;
            //            printf("handle right up :%f,%f\n",e.x,e.y);
            return true;
        }
    }
    return false;
}


//keyboard event
void process_keyboard_event(SDL_KeyboardEvent& e,SDLWindow* win=NULL)
{
    auto k = e.keysym.sym;
    if(k == SDLK_LSHIFT || k==SDLK_RSHIFT)
    {
        k = VK_SHIFT;
    }
    else if(k == SDLK_LALT||k==SDLK_RALT)
    {
        k = VK_MENU;
    }
    else if(k == SDLK_LCTRL || k==SDLK_RCTRL)
    {
        k = VK_CONTROL;
    }
    
    if(e.type == SDL_KEYDOWN)
    {
        win->OnKeyDown(k, e.keysym.mod, 0, false);
        printf("key down:state:%d,scancode:0x%0x,sym:0x%0x::mod:0x%0x\n",
               e.state,e.keysym.scancode,k,e.keysym.mod);
    }
    else if(e.type == SDL_KEYUP)
    {
        win->OnKeyUp(k, e.keysym.mod);
        printf("key up:state:%d,scancode:0x%0x,sym:0x%0x::mod:0x%0x\n",
               e.state,e.keysym.scancode,k,e.keysym.mod);
    }
}

static uint32_t last_mouse_button_down_time = 0;
static bool is_mouse_button_down = false;

const uint32_t ctrl_key_time = 3000; //5000ms
static bool is_ctrl_key_down = false;

static void ctrl_down(SDLWindow* win)
{
//    auto mod = SDL_GetModState();
    SDL_Keymod mod = KMOD_LCTRL;
    SDL_SetModState(mod);
    
    win->OnKeyDown(VK_CONTROL, mod, 0, false);
    printf("on ctrl key down!\n");
}

static void ctrl_up(SDLWindow* win)
{
//    auto mod = SDL_GetModState();
    SDL_Keymod mod = KMOD_NONE;
    SDL_SetModState(mod);
    
    win->OnKeyUp(VK_CONTROL, mod);
    printf("on ctrl key up!\n");
}

static void mouse_down_cb(uint32_t now, SDLWindow* win)
{
    is_mouse_button_down = true;
    last_mouse_button_down_time = now;
}

static void mosue_up_cb(uint32_t now, SDLWindow* win)
{
    is_mouse_button_down = false;
    if(is_ctrl_key_down)
    {
        ctrl_up(win);
        is_ctrl_key_down = false;
    }
}

void simula_ctrl_check(uint32_t now,SDLWindow* win)
{
    if(!is_mouse_button_down) return;
    if(now - last_mouse_button_down_time > ctrl_key_time &&
       !is_ctrl_key_down)
    {
        printf("check!now:%d,last:%d\n",now,last_mouse_button_down_time);
        is_ctrl_key_down = true;
        ctrl_down(win);
    }
}

#ifndef SDL_TEST

// extern void process_input_event(SDL_Event& e);
std::set<uint32_t> input_events = {
    SDL_MOUSEBUTTONUP,
    SDL_MOUSEBUTTONDOWN,
    SDL_MOUSEMOTION,
    SDL_FINGERUP,
    SDL_FINGERDOWN,
    SDL_KEYDOWN,
    SDL_KEYUP,
    SDL_FINGERMOTION,
};

void process_input_event(SDL_Event& e,SDLWindow* win)
{
    if( e.type == SDL_MOUSEBUTTONUP )
    {
        int x = 0, y = 0;
        SDL_GetMouseState( &x, &y );
        
        win->trans_point_frome_win(x, y);
        
        if(e.button.button == SDL_BUTTON_LEFT)
        {
            printf("mouse click Left up:x:%d,y:%d\n", x,y);
            if(win)
            {
                win->OnMouseClick(mbLeft,0,x,y);
                win->OnMouseUp(mbLeft,0,x,y);
            }
            mosue_up_cb(SDL_GetTicks(),win);
        }
        else if(e.button.button == SDL_BUTTON_RIGHT)
        {
            printf("mouse click Right up:x:%d,y:%d\n", x,y);
            if(win)
            {
                win->OnMouseClick(mbRight,0,x,y);
                win->OnMouseUp(mbRight,0,x,y);
            }
            
        }
        
        // handleKeys( e.text.text[ 0 ], x, y );
    }
    else if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        int x = 0, y = 0;
        SDL_GetMouseState( &x, &y );
        win->trans_point_frome_win(x, y);
        
        if(e.button.button == SDL_BUTTON_LEFT)
        {
            printf("mouse click Left down:x:%d,y:%d\n", x,y);
#ifndef SDL_TEST
            if(win)
            {
                // win->OnMouseClick(mbLeft,0,x,y);
                win->OnMouseDown(mbLeft,0,x,y);
            }
            mouse_down_cb(SDL_GetTicks(),win);
#endif
        }
        else if(e.button.button == SDL_BUTTON_RIGHT)
        {
            printf("mouse click Right down:x:%d,y:%d\n", x,y);
            if(win)
            {
                // win->OnMouseClick(mbRight,0,x,y);
                win->OnMouseDown(mbRight,0,x,y);
            }
        }
    }
    else if(e.type == SDL_MOUSEMOTION)
    {
        int x = 0, y = 0;
        SDL_GetMouseState( &x, &y );
        win->trans_point_frome_win(x, y);
        if(win)
        {
            win->OnMouseMove(0, x, y);
        }
    }
    else if(e.type == SDL_FINGERDOWN)
    {
        //        printf("SDL_FINGERDOWN::touchId:%d,fingerId:%d,pressure:%d\n",e.tfinger.touchId,
        //               e.tfinger.fingerId,e.tfinger.pressure);
        bool r = handle_finger(e.tfinger);
        if(r)
        {
            int x,y;
            x = win->GetWidth() * e.tfinger.x;
            y = win->GetHeight() * e.tfinger.y;
            win->trans_point_frome_win(x, y);
            win->OnMouseDown(mbRight, 0, x, y);
            printf("==handle touch! right click down!,%d,%d\n",x,y);
        }
    }
    else if(e.type == SDL_FINGERUP)
    {
        //        printf("SDL_FINGERUP::touchId:%d,fingerId:%d,pressure:%d\n",e.tfinger.touchId,
        //               e.tfinger.fingerId,e.tfinger.pressure);
        bool r = handle_finger(e.tfinger);
        if(r)
        {
            int x,y;
            x = win->GetWidth() * e.tfinger.x;
            y = win->GetHeight() * e.tfinger.y;
            win->trans_point_frome_win(x, y);
            win->OnMouseClick(mbRight,0,x,y);
            win->OnMouseUp(mbRight,0,x,y);
            printf("==handle touch! right click up!,%d,%d\n",x,y);
        }
        
    }
    else if(e.type == SDL_FINGERMOTION)
    {
        int x,y;
        x = win->GetWidth() * e.tfinger.x;
        y = win->GetHeight() * e.tfinger.y;
        win->trans_point_frome_win(x, y);
        win->OnMouseMove(0, x, y);
    }
    else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
    {
        process_keyboard_event(e.key,win);
    }
}
#endif
