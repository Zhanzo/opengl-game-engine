#version 330 core

in vec2 TexCoords;
in vec3 ourColor;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main() { 
    //vec4 color = vec4(ourColor, 1.0);

    vec3 diffuse = texture(texture_diffuse1, TexCoords).xyz;
    vec3 specular = texture(texture_specular1, TexCoords).xyz;
    FragColor = vec4(diffuse + specular, 1.0); 
}