#ifndef _H_DEVICE_
#define _H_DEVICE_




//#if TARGET_OS_IPHONE||TARGET_IPHONE_SIMULATOR
#include "SDL_opengles2.h"
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
//#else
//#include "GL/glew.h"
//#endif

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

#include "gltexture.hpp"
#include "updateMgr.hpp"





const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

// const int win_width = WIN_WIDTH;
// const int win_height = WIN_HEIGHT;

extern const char* VERTEX_SHADER_FILE;
extern const char* FRAG_SHADER_FILE;



class SHADER
{
    GLuint shader_id;
    const char* vet_file_path;
    const char* frag_file_path;
public:
    SHADER(const char* vet_file, const char* frag_file);
    SHADER();
    void read_shader_file(const char *fname,char *buf);
    void init_shader();
    void use();
    GLint glGetAttribLocation(const GLchar* name);

};

class VAOMGR
{
    //OPENGL Device
    GLuint vbo;
    GLuint vao;
    int num;
public:
    VAOMGR(int n);
    void gen_vao_vbo(GLfloat vertices[],size_t size,int pos_loc=0,int tex_loc=1);

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
    int position_loc;
    int tex_coord_loc;

    // GLuint texture;
public:
    Device(int w=WIN_WIDTH, int h=WIN_HEIGHT);
    void set_texture(GLTexture* tex);
    void update_texture();
    // void update_texture(void* img_buf,int w,int h,bool is_rgba=false);

    void init_render();
    void update(unsigned int cur_tick);
    void render(unsigned int cur_tick);
    void before_draw();
    void after_draw();

    void release();
};


Device* create_gl_device(int w, int h);


#endif
