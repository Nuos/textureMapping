#version 330

in vec3 ver_color;
in vec2 ver_uv;

uniform sampler2D cube_texture;

out vec4 frag_color;

void main(void)
{
    frag_color = texture(cube_texture, ver_uv);
}

