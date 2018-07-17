
//gen by gen_shaders.py
#ifndef _SHADERS_H_
#define _SHADERS_H_
const char* const simple_vs_shader="#version 330 core\n\
layout (location = 0) in vec3 position;\n\
layout (location = 1) in vec2 tex_coord;\n\
out vec2 TexCoord;\n\
void main()\n\
{\n\
gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
TexCoord = tex_coord;\n\
}";
const char* const simple_fs_shader="/*\n\
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
const char* const simple_es_vs_shader="#version 300 es\n\
in vec3 position;\n\
in vec2 tex_coord;\n\
out vec2 TexCoord;\n\
void main()\n\
{\n\
gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
TexCoord = tex_coord;\n\
}";
const char* const simple_es_fs_shader="#version 300 es\n\
precision mediump float;\n\
in vec2 TexCoord;\n\
out vec4 color;\n\
uniform sampler2D ourTexture;\n\
void main()\n\
{\n\
// color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n\
vec2 tex_coord = vec2(TexCoord.x,1.0f-TexCoord.y);\n\
color = texture(ourTexture, tex_coord);\n\
}";
const char* const object2d_vs_sharder="//object2d_vs.glsl\n\
#version 330 core\n\
layout (location = 0) in vec3 position;\n\
layout (location = 1) in vec2 tex_coord;\n\
uniform mat4 transform; //pos,scale?, to world coord\n\
uniform mat4 ortho; // to -1.0~1.0\n\
\n\
out vec2 TexCoord;\n\
// out vec3 pos_col;\n\
void main()\n\
{\n\
gl_Position = ortho * transform * vec4(position, 1.0);\n\
TexCoord = tex_coord;\n\
// pos_col = col;\n\
}";
const char* const object2d_fs_sharder="//object2d_fs.glsl\n\
#version 330 core\n\
in vec2 TexCoord;\n\
// in vec3 pos_col;\n\
out vec4 color;\n\
uniform sampler2D ourTexture;\n\
void main()\n\
{\n\
color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n\
// vec2 tex_coord = vec2(TexCoord.x,1.0f-TexCoord.y);\n\
// color = texture(ourTexture, tex_coord);\n\
// color = vec4(pos_col,1.0f);\n\
}";
#endif
