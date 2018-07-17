//object2d_vs.glsl
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
uniform mat4 transform; //pos,scale?, to world coord
uniform mat4 ortho; // to -1.0~1.0 

out vec2 TexCoord;
// out vec3 pos_col;
void main()
{
	gl_Position = ortho * transform * vec4(position, 1.0);
	TexCoord = tex_coord;
	// pos_col = col;
}