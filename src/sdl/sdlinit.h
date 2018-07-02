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
extern int g_scale_win;

void scale_view(ViewRect& src,ViewRect& dst,int mode);
void set_scale_mode(int mode);

SDL_Window* create_sdl_window(const char* title,int w,int h);

#endif /* sdlinit_h */
