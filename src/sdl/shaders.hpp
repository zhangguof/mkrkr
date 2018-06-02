
//gen by gen_shaders.py
#ifndef _SHADERS_H_
#define _SHADERS_H_
const char* simple_vs_shader="#version 330 core\n\
layout (location = 0) in vec3 position;\n\
layout (location = 1) in vec2 tex_coord;\n\
out vec2 TexCoord;\n\
void main()\n\
{\n\
gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
TexCoord = tex_coord;\n\
}";
const char* simple_fs_shader="/*\n\
fragmentShader.glsl\n\
test3d\n\
\n\
Created by tony on 16/3/20.\n\
Copyright © 2016年 tony. All rights reserved.\n\
*/\n\
#version 330 core\n\
in vec2 TexCoord;\n\
out vec4 color;\n\
uniform sampler2D ourTexture;\n\
void main()\n\
{\n\
// color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n\
vec2 tex_coord = vec2(TexCoord.x,1.0f-TexCoord.y);\n\
color = texture(ourTexture, tex_coord);\n\
}";
#endif
