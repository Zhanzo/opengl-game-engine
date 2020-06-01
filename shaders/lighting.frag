#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3  object_color;
uniform vec3  light_color;
uniform vec3  light_pos;
uniform vec3  view_pos;
uniform float exp;

void main() {
    vec3 norm        = normalize(Normal);
    vec3 light_dir   = normalize(light_pos - FragPos);
    vec3 view_dir    = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-light_dir, norm);

    // ambient
    float ambient_intensity = 0.1;
    vec3  ambient           = ambient_intensity * light_color;

    // diffuse
    float diff    = max(dot(norm, light_dir), 0.0);
    vec3  diffuse = diff * light_color;

    // specular
    float specular_intensity = 0.5;
    float spec               = pow(max(dot(view_dir, reflect_dir), 0.0), exp);
    vec3  specular           = specular_intensity * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * object_color;
    FragColor   = vec4(result, 1.0);
}