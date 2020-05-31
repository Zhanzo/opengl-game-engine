#version 330 core
out vec4 FragColor;  

//in vec3 our_color;
in vec2 our_tex_coord;

uniform sampler2D texture1;
uniform sampler2D texture2;
  
void main() {
    FragColor = mix(texture(texture1, our_tex_coord), texture(texture2, our_tex_coord), 0.2);
}