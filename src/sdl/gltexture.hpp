#ifndef _GL_TEXTURE_H_
#define _GL_TEXTURE_H_
#include "GL/glew.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include <assert.h>
#include <string.h>

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
		buf_size = width * height * format_size * type_size;
		pitch = width * format_size * type_size; //one row size;	
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

	}
	void copybuf(void *src)
	{
		assert(pixels && src && buf_size!=0);
		memcpy(pixels,src,buf_size);
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

};


#endif