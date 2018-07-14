#include "gltexture.hpp"

GLTexture::GLTexture()
{
	id = width = height = 0;
	pixels = NULL;
	buf_size = 0;
	format_size = format==GL_RGB?3:4;
	type_size = type == GL_UNSIGNED_BYTE?sizeof(GLubyte):sizeof(GLuint);
	pitch = 0;
	glGenTextures(1,&id);

	is_bind = false;
}

GLTexture::GLTexture(int w, int h)
{
	width = w;
	height = h;
	pixels = NULL;
	setup_size(w,h);
	new_buf();

	glGenTextures(1,&id);
	is_bind = false;
}

void GLTexture::update_texture()
{
	if(!pixels || !is_bind)
		return;
    // glBindTexture(GL_TEXTURE_2D, id);
    if(!has_init_prams)
    {
    	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	has_init_prams = true;
    }
    if(is_dirty)
    {
    	glTexImage2D(GL_TEXTURE_2D, 0, 
		inter_format, width, height,0, 
		format, type, pixels);
		is_dirty = false;
    }


    // glGenerateMipmap(GL_TEXTURE_2D);

    // glBindTexture(GL_TEXTURE_2D, 0);
}