#version 300 es
precision mediump float;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
void main()
{
	// color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vec2 tex_coord = vec2(TexCoord.x,1.0f-TexCoord.y);
	color = texture(ourTexture, tex_coord);
}
