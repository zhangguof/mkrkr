/* 
  fragmentShader.glsl
  test3d

  Created by tony on 16/3/20.
  Copyright © 2016年 tony. All rights reserved.
*/
#version 330 core
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
void main()
{
	// color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vec2 tex_coord = vec2(TexCoord.x,1.0f-TexCoord.y);
	color = texture(ourTexture, tex_coord);
}
