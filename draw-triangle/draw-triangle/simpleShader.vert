#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uv;

out vec3 ver_color;
out vec2 ver_uv;

void main(void)
{
    gl_Position = vec4(position, 1.0);
    
    ver_color = color;
    ver_uv = uv;
}

