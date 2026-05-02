#version 430
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoords;

out vec3 v_Normal;
out vec3 v_WorldPos;
out vec2 v_Texcoords;

uniform mat4 u_world;
uniform mat4 u_proj;
uniform mat4 u_view;

void main() {
    vec4 worldPos = u_world * vec4(a_position, 1.0);
    gl_Position = u_proj * u_view * worldPos;
    v_WorldPos = worldPos.xyz;
    v_Normal = normalize(transpose(inverse(mat3(u_world))) * a_normal);
    v_Texcoords = a_texcoords;
}