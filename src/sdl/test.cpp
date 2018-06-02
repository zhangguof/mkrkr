#include <SDL.h>
#include "GLDevice.hpp"
#include "imgbuf.h"
#include "gltexture.hpp"


extern int load_bmp(GLTexture* tex,char* filename);
extern unsigned char* fill_color(unsigned int col,int w, int h);
Device* g_dev;
GLTexture* tex1;
GLTexture* tex2;
bool is_img1 = false;

unsigned int last_time = 0;

void render(unsigned int interval)
{

	g_dev->render(interval);
	unsigned int now = time_now();
	if(now - last_time >= 500)
	{
		if(is_img1)
		{
			printf("change to img 2\n");
			g_dev->set_texture(tex2);
			is_img1 = false;
		}
		else
		{
			printf("change to img 1\n");
			g_dev->set_texture(tex1);
			is_img1 = true;
		}
		last_time = time_now();
	}


	//SDL_Delay(1000);
}

int main(int argc, char* args[])
{

	
	SDL_Window* win = NULL;
	// SDL_Surface* screen_surface = NULL;
	//init
	if(init_sdl()<0)
		return 0;
	win  = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN );
	if(init_gl_context(win)<0)
	{
		printf("init_gl_context error!\n");
		return 0;
	}
	
	tex1 = new GLTexture();
	tex2 = new GLTexture();

	load_bmp(tex1,"test.bmp");
	// load_bmp(tex2,"sdl/test2.bmp");
	unsigned char* buf = fill_color(0x000000FF,WIN_WIDTH,WIN_HEIGHT);
	tex2->set_buf(buf,WIN_WIDTH,WIN_HEIGHT);
	Device* p_dev = new Device();
	// regist_objupdate(p_dev);
	p_dev->init_render();

	regist_update(render);
	p_dev->set_texture(tex2);
	is_img1 = false;

	p_dev->init_render();
	g_dev = p_dev;

	// p_dev->update_texture(img_buf,w,h);
	sdl_loop();


    // SDL_FillRect(screen_surface,NULL,SDL_MapRGB(screen_surface->format, 0xff, 0x00, 0x00) );
 //Update the surface
    // SDL_UpdateWindowSurface( win );
    // printf("test?\n");
    // SDL_Delay( 5000 );
    delete tex1;
    delete tex2;

	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}