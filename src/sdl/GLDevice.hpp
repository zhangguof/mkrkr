#ifndef _H_DEVICE_
#define _H_DEVICE_
// #include "T3D.hpp"

#include "GL/glew.h"
#include "SDL.h"
#include "SDL_opengl.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

#include <SOIL.h>
// #include "imgbuf.h"
#include "gltexture.hpp"





const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

// const int win_width = WIN_WIDTH;
// const int win_height = WIN_HEIGHT;

extern const char* VERTEX_SHADER_FILE;
extern const char* FRAG_SHADER_FILE;

// class ImgBuf
// {

// 	int w,h;
// 	void* buf;
// 	bool is_rgba;
// 	unsigned int format;
// 	ImgBuf(void* buffer,int width, int height,
// 	bool _rgba,unsigned int _format):
// 	buf(buffer),w(width),h(height),is_rgba(_rgba),format(_format)
// 	{}
// }


class UpdateObj
{
public:
    virtual void update(unsigned int value)=0;
};

class SHADER
{
    GLuint shader;
    const char* vet_file_path;
    const char* frag_file_path;
public:
    SHADER(const char* vet_file, const char* frag_file);
    SHADER();
    void read_shader_file(const char *fname,char *buf);
    void init_shader();
    void use();

};

class VAOMGR
{
    //OPENGL Device
    GLuint vbo;
    GLuint vao;
    int num;
public:
    VAOMGR(int n);
    void gen_vao_vbo(GLfloat vertices[],size_t size);

    void bind();
    void ubind();
    void draw();
    ~VAOMGR();

};

class Device: public UpdateObj
{
    int win_width;
    int win_height;
    const static int buff_num = 2;
    //DrawBuffer<GLuint> buff[buff_num];

    //img buf
    // ImgBuf* img_buf;
    // Texture* last_texture;
    GLTexture* cur_texture;

    //opengl
    VAOMGR *vao;
    SHADER *shader;

    // GLuint texture;
public:
    Device(int w=WIN_WIDTH, int h=WIN_HEIGHT);
    void set_texture(GLTexture* tex);
    void update_texture();
    // void update_texture(void* img_buf,int w,int h,bool is_rgba=false);

    void init_render();
    void update(unsigned int interval);
    void render(unsigned int interval);
    void before_draw();
    void after_draw();

    void release();
};


typedef void (*UpdateFunc)(unsigned int interval);
typedef void (UpdateObj::*ObjUpdateFunc)(unsigned int interval);
//typedef std::pair<UpdateObj*,ObjUpdateFunc> UpdatePair;

class CBMgr
{
    std::vector<UpdateFunc> v;
    //std::vector<UpdatePair> v2;
    std::vector<UpdateObj*> v2;

public:
    void add_callback(UpdateFunc f)
    {
        v.push_back(f);
    }
    void add_callback2(UpdateObj* p)
    {
        v2.push_back(p);
    }
    void update(unsigned int interval)
    {
        for (auto i = v.begin(); i != v.end(); ++i)
        {
            (*i)(interval);
        }

        for (auto i = v2.begin(); i != v2.end(); ++i)
        {
            // auto ptr_obj = i->first;
            // auto f = i->second;
            // (ptr_obj->*f)(interval);
            (*i)->update(interval);
        }

    }
};


int  init_sdl();
int init_gl_context(SDL_Window* win);
void sdl_loop();
void regist_update(UpdateFunc f);
void regist_objupdate(UpdateObj* p);

unsigned int time_now();

Device* create_gl_device(int w, int h);


#endif
