#include <stdio.h>
#include "SDL.h"
#include "GLDevice.hpp"
#include <boost/filesystem.hpp>
#include "ffStream.hpp"
#include "ffvideo.hpp"
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

// extern Device* g_dev;
GLTexture* img_tex1;
std::shared_ptr<FrameBuffer> pbf;
float fps = 29.97;
uint32_t one_frame_time = 1000 / 29.97;


VideoPlayer* vp=nullptr;

static void render(uint32_t cur_tick)
{
    if(vp)
        vp->update(cur_tick);
	if(g_dev)
		g_dev->render(cur_tick);
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

void img_buff_fill_cb(uint8_t* buf,int w,int h,int size)
{
    if(img_tex1)
    {
        assert(size == img_tex1->format_size);
        img_tex1->set_buf(buf,w,h);
    }
}


void test_ff_video_play()
{
     // std::string fname = "01_01_d.mpg";
   std::string fname = "06_04_d.mpg";
    // auto fs = std::make_shared<ffStream>(fname);
    // //decode all
    // int f_cnt = 0;
    // uint32_t now_t = time_now();
    // while(!fs->video_decoded_end)
    // {
    //     int r = fs->video_decode_nframe();
    //     if(r>0)
    //         f_cnt++;
    // }
    // uint32_t cost_time = time_now() - now_t;
    // printf("decode all cost:%dms,frames:%d,frame_pre_time:%f\n", cost_time , f_cnt, cost_time*1.0 / f_cnt);
    // pbf = fs->get_video_buffer();
    int w,h;
    w = 1024;
    h = 768;
    
    int one_frame_size = w * h * 4;
    printf("one_frame_size:%d\n", one_frame_size);
    uint8_t* buf = nullptr;
    
    img_tex1 = new GLTexture();
    // img_tex2 = new GLTexture();
//    img_tex1->set_format(GL_RGBA);
    // img_tex2->set_format(GL_RGB);
    
    // int size = pbf->read(&buf,one_frame_size);
    // assert(size > 0);
    // img_tex1->set_buf(buf,w,h);
    // size = pbf->read(&buf,one_frame_size);
    // assert(size > 0);
    // img_tex2->set_buf(buf,w,h);
    g_dev->set_texture(img_tex1);
    vp = new VideoPlayer();
    vp->set_cb(img_buff_fill_cb);
    vp->open(fname,false);
    vp->set_loop(true);
    
    vp->play();
}

static void test_img()
{
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
}

extern void al_test_play();
extern "C" int init_ffmpeg();

int check_info()
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
    return 0;
}

extern "C" int init_core_test(int argc,char *argv[])
{
    init_ffmpeg();
    
	int w = win_width;
	int h = win_height;
	char *base_path = SDL_GetBasePath();
	SDL_Log("core test start!");
	SDL_Log("base_path:%s",base_path);
    boost::filesystem::path p = boost::filesystem::current_path();
    SDL_Log("cur path from filesystem:%s",p.c_str());
	init_sdl_test(w, h);
    
	g_dev = create_gl_device(w, h);
	g_dev->init_render();
    
//    test_img();
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    
    check_info();


    
//voice test
    // std::string fname = "a0001.ogg";
    // auto ps = std::make_shared<ffStream>(fname);
    al_test_play();
    
 //video play test
    SDL_Log("start video test!");
    test_ff_video_play();

    

	regist_update(render);
    return 0;

}
