#include "GLDevice.hpp"
// #include "shaders.hpp"

#ifndef SDL_TEST
#include "tjsCommHead.h"
#include "WindowImpl.h"

extern tTJSNI_Window * TVPMainWindow;

#endif
#include "sdlinit.h"

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

CBMgr* cb_mgr;
// GLFWwindow* glfw_window;

//OpenGL context
SDL_GLContext gContext;

static bool InitSdl = false;

static int check_info(SDL_Window* win);

//0:as system
//1 4:3
//2 16:9

winScaleMode g_scale_win = AUTO;
struct ScaleArg
{
    double rate;
    int mode;
} g_scale_modes[]= {{0.0f,0},{4.0/3.0,1},{16.0/9.0,2}};

void set_scale_mode(winScaleMode mode)
{
    g_scale_win = mode;
}

//src: devcie bound
//dst: glview arg
//rate = w/h
void scale_view(ViewRect& src,ViewRect& dst,winScaleMode mode)
{
    //device width:height 568:320 16:9
    if(mode == AUTO) return;
//    assert(mode== || mode==2);
    dst = src;
    double rate = g_scale_modes[mode].rate;
    if(fabs(1.0*src.w/src.h - rate)<0.01)
    {
        SDL_Log("scale view in same rate:%lf",rate);
        return;
    }
    if(mode == MODE_4_3)
    {
 
        //left right clip
        dst.x = src.x + int((src.w - rate*src.h)/2);
        dst.y = src.y;
        
        dst.w = int(src.h * rate);
        dst.h = src.h;
    }
    else if(mode == MODE_16_9)
    {
        //top bottom clip
        dst.x = src.x;
        dst.w = src.w;
        
        
        dst.y = src.y + int((src.h - 1.0/rate * src.w)/2);
        dst.h = int(src.w * 1.0/rate);
    }
}


int init_gles_context(SDL_Window* win)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);	// or 1

    if(!gWindow)
    {
    	gWindow = win;
    }
    //Create context
    gContext = SDL_GL_CreateContext( win );
    if( gContext == NULL )
    {
        printf( "OpenGLES context could not be created! SDL Error: %s\n", SDL_GetError() );
        return -1;
    }
    int w,h;
    SDL_GL_GetDrawableSize(win, &w, &h);
    ViewRect src = {0,0,w,h};
    ViewRect dst = src;
//    set_scale_mode(1);//4:3
    if(g_scale_win != AUTO)
    {
        scale_view(src, dst, g_scale_win);
    }
//    glViewport(0, 0, w, h);
    glViewport(dst.x, dst.y, dst.w, dst.h);
    SDL_Log("glViewport::%d,%d,%d,%d",dst.x, dst.y, dst.x+dst.w, dst.y+dst.h);

    // glewExperimental = GL_TRUE;
    // glewInit();

    const char *version = (const char *)glGetString(GL_VERSION);
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char* render = (const char*)glGetString(GL_RENDERER);
    printf("OpenGLES version:%s\nOpengles Vendor:%s\nRender:%s\n",version,vendor,render);
    check_info(win);
    return 0;

}

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#pragma message("using OpenGLES context!")
int init_gl_context(SDL_Window* win)
{
    return init_gles_context(win);
}
#else
#pragma message("using OpenGL context!")
int init_gl_context(SDL_Window* win)
{
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );


    if(!gWindow)
    {
    	gWindow = win;
    }
    //Create context
    gContext = SDL_GL_CreateContext( win );
    if( gContext == NULL )
    {
        printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
        return -1;
    }

    glewExperimental = GL_TRUE;
    glewInit();

    const char *version = (const char *)glGetString(GL_VERSION);
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    printf("OpenGL version:%s\nOpengl Vendor:%s\n",version,vendor);
    
    return 1;
}

#endif


static int check_info(SDL_Window* win)
{
    int i, display_mode_count;
    SDL_DisplayMode mode;
    Uint32 f;
    
    SDL_Log("SDL_GetNumVideoDisplays(): %i", SDL_GetNumVideoDisplays());
    
    display_mode_count = SDL_GetNumDisplayModes(0);
    if (display_mode_count < 1) {
        SDL_Log("SDL_GetNumDisplayModes failed: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("SDL_GetNumDisplayModes: %i", display_mode_count);
    
    for (i = 0; i < display_mode_count; ++i) {
        if (SDL_GetDisplayMode(0, i, &mode) != 0) {
            SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
            return 1;
        }
        f = mode.format;
        
        SDL_Log("Mode %i\tbpp %i\t%s\t%i x %i", i,
                SDL_BITSPERPIXEL(f), SDL_GetPixelFormatName(f), mode.w, mode.h);
    }
    
    
    // Declare display mode structure to be filled in.
    SDL_DisplayMode current;
    
    // Get current display mode of all displays.
    for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i){
        
        int should_be_zero = SDL_GetCurrentDisplayMode(i, &current);
        
        if(should_be_zero != 0)
            // In case of error...
            SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
        
        else
            // On success, print the current display mode.
            SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", i, current.w, current.h, current.refresh_rate);
        
    }
    int w,h;
    SDL_GetWindowSize(win,&w,&h);
    SDL_Log("Get windwos Size:%d,%d",w,h);
    return 0;
}


int init_sdl()
{
	if(InitSdl) return 0;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0)
    {
        printf("sdl init error:%s\n",SDL_GetError());
        return -1;
    }
    InitSdl = true;
    return 0;
}

SDL_Window* create_sdl_window(const char* title,int w,int h)
{
    SDL_Window* win = NULL;
    win = SDL_CreateWindow(
        title,                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        w,                               // width, in pixels
        h,                               // height, in pixels
        SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI // flags - see below
        );
    return win;
}




#ifndef SDL_TEST
extern void process_use_envet(SDL_UserEvent& e);
// extern void process_input_event(SDL_Event& e);
void process_input_event(SDL_Event& e)
{
    SDLWindow* win = NULL;
    if(TVPMainWindow)
    	win = TVPMainWindow->GetForm();
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
}
#endif

void sdl_event_update(bool &quit)
{
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            quit = true;
            return;
        }
#ifndef SDL_TEST
        else if( e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
        {
            //Handle keypress with current mouse position
            process_input_event(e);
        }
        else if( e.type == SDL_USEREVENT)
        {
            process_use_envet(e.user);
        }
#endif
    }
}

const double g_fps = 60.0f;
const double g_event_fps = 20.0f;
uint32_t frame_pre_time = uint32_t(1000.0/g_fps);
uint32_t event_pre_time = uint32_t(1000.0/g_event_fps);

extern "C" void sdl_loop()
{
    uint32_t last_time = 0;
    uint32_t last_event_time = 0;
    uint32_t now;
    uint32_t sleep_time = 0;
//    unsigned int interval;
    bool quit = false;
//    SDL_Event e;

    while( !quit )
    {
        sleep_time = 0;
        now = time_now();
        //Handle events on queue
        if(now - last_event_time > event_pre_time)
        {
            sdl_event_update(quit);
            last_event_time = now;
        }

//        interval = now - last_time;
        //render();
        if(cb_mgr)
        	cb_mgr->update(now);
        

        last_time = now;
        uint32_t cost_time = time_now() - now;
        if(cost_time < frame_pre_time)
        {
            sleep_time = frame_pre_time - cost_time;
//            printf("sleep:%u\n",sleep_time);
        }
        
        SDL_Delay(sleep_time);
    }

}

void regist_update(UpdateFunc f)
{
	if(!cb_mgr)
	{
		cb_mgr = new CBMgr;
	}
    cb_mgr->add_callback(f);
}

void regist_objupdate(UpdateObj* p)
{
	if(!cb_mgr)
	{
		cb_mgr = new CBMgr;
	}
    cb_mgr->add_callback2(p);
}

unsigned int time_now()
{
    return SDL_GetTicks();
}

Device* create_gl_device(int w, int h){
	return new Device(w,h);
}


//test code
extern "C" void init_sdl_test(int w,int h)
{
	if(!InitSdl)
		init_sdl();
		SDL_Window* win = SDL_CreateWindow(
        "OpenGLES TEST",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        w,                               // width, in pixels
        h,                               // height, in pixels
        SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI                  // flags - see below
    );
    init_gl_context(win);
}
