#ifndef _GL_TEXTURE_H_
#define _GL_TEXTURE_H_

#include "SDL.h"

#include <assert.h>
#include <string.h>

#if TARGET_OS_IPHONE||TARGET_IPHONE_SIMULATOR
#include "SDL_opengles2.h"
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include "GL/glew.h"
#endif
#include "SDL_opengl.h"



//GLAPI void GLAPIENTRY glTexImage2D 
//(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
class GLTexture
{
public:
	GLenum inter_format =  GL_RGBA;//or GL_RGBA
	// GLenum format =  GL_RGBA; //or GL_RGBA
	GLenum format = GL_BGRA;
	GLenum type = GL_UNSIGNED_BYTE;

	GLuint id;
	int width;
	int height;
	void* pixels;
	int buf_size;
	int format_size, type_size;
	int pitch; //row size?
	bool is_bind;
	bool has_init_prams;
	bool is_dirty;

	GLTexture();
	GLTexture(int w,int h);
	void setup_size(int w,int h)
	{
		width = w;
		height = h;
		setup_size();
	}
	void setup_size()
	{
		format_size = (format==GL_RGB?3:4);
		type_size   = (type == GL_UNSIGNED_BYTE?sizeof(GLubyte):sizeof(GLuint));
		int pre_buf_size = buf_size;
		buf_size = width * height * format_size * type_size;
		pitch = width * format_size * type_size; //one row size;
		if(pre_buf_size!=buf_size && pixels!=NULL)
		{
			free(pixels);
			pixels = NULL;
			new_buf(); 
		}
		has_init_prams = false;
		is_dirty = true;
	}
	~GLTexture()
	{
		if(is_bind) unbind();
		if(pixels)
		{
			free(pixels);
			// delete []pixels;
			pixels = NULL;
		}
		glDeleteTextures(1,&id);
		printf("release Texture!\n");
	}
	void new_buf()
	{
		assert(width!=0 && height!=0);
		if(pixels) return;
		// pixels =  new GLubyte[buf_size];
		pixels = malloc(buf_size);
		is_dirty = true;

	}
	void copybuf(void *src)
	{
		assert(pixels && src && buf_size!=0);
		memcpy(pixels,src,buf_size);
		is_dirty = true;
	}
	void set_buf(void *b,int w,int h)
	{
		// pixels = b;
		setup_size(w,h);
		// pixels = (GLubyte*) b;
		if(pixels==NULL)
		{
			new_buf();
		}
		copybuf(b);
		update_texture();
	}
	void set_buf(void* b)
	{
		if(width == 0 || height == 0) return;
		copybuf(b);
		update_texture();
	}
	void set_type(GLenum t)
	{
		type = t;
		setup_size();
	}
	void set_format(GLenum f)
	{
		format = f;
		setup_size();
	}
	void update_texture();//after set buf.
	void bind()
	{
		assert(id!=0 && !is_bind);
		glBindTexture(GL_TEXTURE_2D,id);
		is_bind = true;
	}
	void unbind()
	{
		assert(is_bind);
		glBindTexture(GL_TEXTURE_2D,0);
		is_bind = false;
	}
	void lock()
	{
		//ready to modify buffer;
		is_dirty = true;
	}
	void unlock()
	{
		
	}

};


#endif
