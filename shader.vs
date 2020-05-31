#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;
//layout (location = 2) in vec2 in_tex_coord;
  
//out vec3 our_color;
out vec2 our_tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
    //our_color = in_color;
    our_tex_coord = in_tex_coord;
}  