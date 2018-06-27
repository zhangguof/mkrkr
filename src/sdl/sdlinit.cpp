#include "GLDevice.hpp"
// #include "shaders.hpp"

#ifndef SDL_TEST
#include "tjsCommHead.h"
#include "WindowImpl.h"

extern tTJSNI_Window * TVPMainWindow;

#endif

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

CBMgr* cb_mgr;
// GLFWwindow* glfw_window;

//OpenGL context
SDL_GLContext gContext;

static bool InitSdl = false;

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

extern void process_use_envet(SDL_UserEvent& e);



void sdl_loop()
{
    unsigned int last_time = 0;
    unsigned int now;
    unsigned int interval;
    bool quit = false;
    SDL_Event e;

    #ifndef SDL_TEST
    SDLWindow* win = NULL;
    if(TVPMainWindow)
    	win = TVPMainWindow->GetForm();
    #endif

    while( !quit )
    {
        now = time_now();
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
                return;
            }
            //Handle keypress with current mouse position
            else if( e.type == SDL_MOUSEBUTTONUP )
            {
                int x = 0, y = 0;
                SDL_GetMouseState( &x, &y );
                if(e.button.button == SDL_BUTTON_LEFT)
                {
                    printf("mouse click Left up:x:%d,y:%d\n", x,y);
					#ifndef SDL_TEST                   
                    if(win)
                    {
                    	win->OnMouseClick(mbLeft,0,x,y);
                    	win->OnMouseUp(mbLeft,0,x,y);
                    }
                    #endif
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
            else if( e.type == SDL_USEREVENT)
            {
            	process_use_envet(e.user);
            }
        }
        interval = now - last_time;

        //Render quad
        //render();
        if(cb_mgr)
        	cb_mgr->update(interval);
        
        // //Update screen
        // SDL_GL_SwapWindow( gWindow );
        last_time = now;
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

inline unsigned int time_now()
{
    return SDL_GetTicks();
}

Device* create_gl_device(int w, int h){
	return new Device(w,h);
}
