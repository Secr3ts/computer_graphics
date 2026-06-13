#version 420 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec2 tex;

layout (location = 3) in mat4 i_model; // instancing hardware

out vec3 f_pos;
out vec3 f_nor;
out vec2 f_tex;

uniform mat4 view;
uniform mat4 proj;
uniform sampler2D texture_diffuse;
uniform bool use_tex;
uniform float disp_strength;

void main() {
    vec3 displaced_pos = pos;
    if (use_tex) {
        float h = texture(texture_diffuse, tex).r * 2.0 - 1.0;
        displaced_pos += nor * (h * disp_strength);
    }

    f_pos = vec3(i_model * vec4(displaced_pos, 1.0));

    f_nor = mat3(transpose(inverse(i_model))) * nor;

    f_tex = tex;

    gl_Position = proj * view * vec4(f_pos, 1.0);
}


