#version 420 core
out vec4 f_color;

in vec2 tex_coords;
uniform sampler2D screen_tex;

uniform float offset_x;
uniform float offset_y;
uniform float strength;

void main() {
    float sx = offset_x * strength;
    float sy = offset_y * strength;

    vec2 offsets[9] = vec2[](
        vec2(-sx,  sy), vec2(0.0f, sy), vec2(sx,  sy),
        vec2(-sx,  0.0f),    vec2(0.0f, 0.0f),    vec2(sx,  0.0f),
        vec2(-sx, -sy), vec2(0.0f, -sy), vec2(sx, -sy)
    );

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        col += vec3(texture(screen_tex, tex_coords + offsets[i])) * 0.1111; 
    }
    f_color = vec4(col, 1.0);
}