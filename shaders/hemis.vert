#version 420 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 tex;

layout (location = 3) in mat4 i_model;

layout (location = 7) in vec3 i_amb;    // HARWARE INSTANCING
layout (location = 8) in vec3 i_dif;// HARWARE INSTANCING
layout (location = 9) in vec3 i_spec;// HARWARE INSTANCING
layout (location = 10) in float i_shine;// HARWARE INSTANCING


out vec3 f_pos;
out vec3 f_nor;
out vec2 f_tex;

out vec3 m_amb;
out vec3 m_dif;
out vec3 m_spec;
out float m_shine; 

uniform mat4 view;
uniform mat4 proj;

void main() {
    f_pos = vec3(i_model * vec4(pos, 1.0));
    f_nor = mat3(transpose(inverse(i_model))) * nor;
    f_tex = tex;
    gl_Position = proj * view * vec4(f_pos, 1.0);

    m_amb = i_amb;
    m_dif = i_dif;
    m_spec = i_spec;
    m_shine = i_shine;
}