//
//  sdlinit.h
//  mkrkr
//
//  Created by tony on 2018/7/2.
//

#ifndef sdlinit_h
#define sdlinit_h

struct ViewRect
{
    int x,y;
    int w,h;
};

enum winScaleMode
{
    AUTO,
    MODE_4_3,
    MODE_16_9
};

extern winScaleMode g_scale_win;

void scale_view(ViewRect& src,ViewRect& dst,winScaleMode mode);


void set_scale_mode(winScaleMode mode);

SDL_Window* create_sdl_window(const char* title,int w,int h);


int  init_sdl();
int init_gl_context(SDL_Window* win);
extern "C" void sdl_loop();


unsigned int time_now();

#endif /* sdlinit_h */
