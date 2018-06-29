#include <stdio.h>
#include "SDL.h"
#include "GLDevice.hpp"
extern "C"
{
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

extern void init_test(int w,int h);
const int win_width = 1024;
const int win_height = 768;

extern "C" void init_sdl_test(int w,int h);
Device* g_dev = nullptr;

static void render(uint32_t interval)
{
	if(g_dev)
		g_dev->render(interval);
}


unsigned char* fill_color(unsigned int col,int w, int h)
{
	int size = w * h * sizeof(unsigned char) * 4;
	unsigned int * buf = (unsigned int *)malloc(size);
	// unsigned char r,g,b,a;
	// r = (col & 0xFF);
	// g = ((col>>8) & 0xFF);
	// b = ((col >> 16) & 0xFF);
	// a = (col >> 24);
	for(int i=0;i< w*h;++i)
	{
		buf[i] = col;
	}
	return (unsigned char *) buf;

}
extern "C" int init_core_test(int argc,char *argv[])
{
	int w = win_width;
	int h = win_height;
	char *base_path = SDL_GetBasePath();
	SDL_Log("core test start!");
	SDL_Log("base_path:%s",base_path);
	init_sdl_test(w, h);
	g_dev = create_gl_device(w, h);
	g_dev->init_render();
	GLTexture* tex = new GLTexture();
	g_dev->set_texture(tex);
//    tex->set_format(GL_RGBA);
	
//    uint8_t* buf = fill_color(0x0000FF,w,h);//blue?
    int width, height, nrChannels;
    unsigned char *data = stbi_load("test0.png", &width, &height, &nrChannels, 0);
    SDL_Log("load img test0.png,w:%d,h:%d,c:%d",
            width,height,nrChannels);
    
	tex->set_buf(data,width,height);
//    free(buf);
    stbi_image_free(data);

	regist_update(render);
    return 0;

}
