#ifndef _H_DEVICE_
#define _H_DEVICE_

#include "SDL.h"

#if TARGET_OS_IPHONE||TARGET_IPHONE_SIMULATOR
#include "SDL_opengles2.h"
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include "GL/glew.h"
#endif


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
#include <map>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>





const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

// const int win_width = WIN_WIDTH;
// const int win_height = WIN_HEIGHT;

extern const char* VERTEX_SHADER_FILE;
extern const char* FRAG_SHADER_FILE;



class SHADER
{
public:
    GLuint shader_id;
    const char* vet_file_path;
    const char* frag_file_path;
    const char* vet_buf;
    const char* frag_buf;
    // std::map<std::string, GLint> uniform_mat;
public:
	// void add_uniform_mat(std::string name)
	// {
	// 	uniform_mat[name] = 0;
	// }
	void set_mat(std::string name,glm::mat4& m4)
	{
		GLint loc =glGetUniformLocation(shader_id, name.c_str());
		assert(loc !=-1);

		glUniformMatrix4fv(loc, 1, GL_FALSE, 
							   glm::value_ptr(m4));
	}

	// void get_uniform_loc(std::string name)
	// {
	// 	if(shader_id)
	// 	{
	// 		GLint loc =glGetUniformLocation(shader_id, name.c_str());
	// 		assert(loc!=-1);
	// 		uniform_mat[name] = loc;
	// 		printf("get uiniform loc:%d\n",loc);
	// 	}
	// }
	// void init_all_uniform()
	// {
	// 	for(auto item:uniform_mat)
	// 	{
	// 		get_uniform_loc(item.first);
	// 		printf("init uniform_mat:%s\n",item.first.c_str());
	// 	}
	// }


	SHADER(const char* _vet_buf , 
           const char* _frag_buf );
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
	void gen_vao_with_pos_color(GLfloat vertices[],size_t size);

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
