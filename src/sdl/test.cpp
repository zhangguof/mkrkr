#include <SDL.h>
#include "GLDevice.hpp"
#include "imgbuf.h"
#include "gltexture.hpp"
#include "safeQueue.hpp"
#include "stdio.h"

#include <thread>
#include <chrono>

#include "ffStream.hpp"
#include "sdlAudio.hpp"
#include "alDevice.hpp"

// extern int init_ffmpeg();

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
extern int play_wav();
extern "C"
{

extern int init_ffmpeg();
extern int open_audio_stream(std::string fname, 
uint8_t** stream, int &len, 
int &freq, int &chs, AVSampleFormat& format);
	
}


// int count = 0;
// bool quit = false;
// SafeQueue<int> q;
// void push_thread()
// {
// 	while(count<10)
// 	{
// 		q.push(count);
// 		printf("push %d\n",count);
// 		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
// 		count++;
// 	}
// 	quit = true;
// 	printf("quit!!\n");
// }

// void pop_thread()
// {
// 	int val;
// 	while(!quit)
// 	{
// 		//while(!q.empty())
// 		{
// 			// auto p = q.wait_and_pop();
// 			// printf("pop %d\n",*p);
// 			if(q.try_pop(val))
// 			{
// 				printf("pop %d\n",val);
// 			}
// 		}
// 		std::this_thread::sleep_for(std::chrono::milliseconds(500));
// 	}
// }

// void test_queue()
// {
// 	// int val = 1;
// 	// SafeQueue<int> q;
// 	std::thread t1(push_thread);
//     std::thread t2(pop_thread);
//     t1.join();
//     t2.join();
// 	// for(int i=0;i<3;++i)
// 	// {
// 	// 	q.push(i);
// 	// }
// 	// while(!q.empty())
// 	// {
// 	// 	printf("%d\n",*(q.wait_and_pop()));
// 	// }
// }
extern int init_al(ALCdevice** pdev=nullptr);
extern void al_test_play();
// extern void test_ff_video_play(); 


// extern void test_ff_loop(uint32_t interval);

// extern Device* g_dev;
GLTexture* img_tex1;
GLTexture* img_tex2;
std::shared_ptr<FrameBuffer> pbf;
float fps = 29.97;
uint32_t one_frame_time = 1000 / 29.97;
int cnt = 0;
void test_ff_loop(uint32_t interval)
{
	static uint32_t last_time = 0;
	uint32_t now = time_now();
	if(now - last_time >= one_frame_time)
	{
		// printf("update:%d,%d\n", now - last_time,cnt);
		uint8_t* buf = nullptr;
		int w = 1024;
		int h = 768;
		cnt++;

		int size = pbf->read(&buf,w*h*4);
		// assert(size > 0);
		if(size > 0)
			img_tex1->set_buf(buf,w,h);
		// if(is_img1)
		// {
		// 	printf("change to img 2\n");
		// 	g_dev->set_texture(tex2);
		// 	is_img1 = false;
		// }
		// else
		// {
		// 	printf("change to img 1\n");
		// 	g_dev->set_texture(tex1);
		// 	is_img1 = true;
		// }
		last_time = time_now();
	}
	g_dev->render(interval);
}

void test_ff_video_play()
{
	std::string fname = "../../video/01_01.mpg";
	// std::string fname = "../../video/06_04_d.mpg";
	auto fs = std::make_shared<ffStream>(fname);
	//decode all
	int f_cnt = 0;
	uint32_t now_t = time_now();
	while(!fs->video_decoded_end)
	{
		int r = fs->video_decode_one();
		if(r>0)
			f_cnt++;
	}
	uint32_t cost_time = time_now() - now_t;
	printf("decode all cost:%dms,frames:%d,frame_pre_time:%f\n", cost_time , f_cnt, cost_time*1.0 / f_cnt);
	pbf = fs->get_video_buffer();
	int w,h;
	w = 1024;
	h = 768;

	int one_frame_size = w * h * 4;
	printf("one_frame_size:%d\n", one_frame_size);
	uint8_t* buf = nullptr;

	img_tex1 = new GLTexture();
	// img_tex2 = new GLTexture();
	img_tex1->set_format(GL_RGBA);
	// img_tex2->set_format(GL_RGB);

	int size = pbf->read(&buf,one_frame_size);
	assert(size > 0);
	img_tex1->set_buf(buf,w,h);
	size = pbf->read(&buf,one_frame_size);
	// assert(size > 0);
	// img_tex2->set_buf(buf,w,h);
	g_dev->set_texture(img_tex1);

}


int main(int argc, char* args[])
{
	// test_queue();
	// unsigned int n = std::thread::hardware_concurrency();
	// printf("(%d)\n", n);
	init_ffmpeg();
	init_sdl();
	int w = 1024;
	int h = 768;

	SDL_Window* win = SDL_CreateWindow(
        "test",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        w,                               // width, in pixels
        h,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );
    if(!win)
    {
 	    printf("win create error:%s\n",SDL_GetError());

    }

    if(win)
    {
    	init_gl_context(win);
    }
	// uint8_t *buf = NULL;
	auto gldevice = create_gl_device(w,h);
	gldevice->init_render();
	g_dev = gldevice;


	// al_test_play();
	test_ff_video_play();
	regist_update(test_ff_loop);

	sdl_loop();


	return 0;
}

int main1(int argc, char* args[])
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