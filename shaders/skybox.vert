#version 420 core
layout (location = 0) in vec3 a_pos;

out vec3 tex_coords;

uniform mat4 view;
uniform mat4 proj;

void main() {
    tex_coords = a_pos;
    vec4 pos = proj * view * vec4(a_pos, 1.0);
    gl_Position = pos.xyww;
}
