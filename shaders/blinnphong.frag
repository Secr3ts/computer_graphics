#version 420 core
out vec4 f_color;

in vec3 f_pos;
in vec3 f_nor;
in vec2 f_tex;

uniform vec3 l_pos;
uniform vec3 v_pos;
uniform vec3 l_col;

uniform vec3 m_amb;
uniform vec3 m_dif;
uniform vec3 m_spec;
uniform float m_shine;

uniform sampler2D texture_diffuse;
uniform bool use_tex;

void main() {
    vec3 ambient = m_amb * l_col;

    vec3 norm = normalize(f_nor);
    vec3 l_dir = normalize(l_pos - f_pos);
    float diff = max(dot(norm, l_dir), 0.0);
    vec3 diffuse = (diff * m_dif) * l_col;

    vec3 v_dir = normalize(v_pos - f_pos);
    vec3 hw_dir = normalize(l_dir + v_dir);
    float spec = pow(max(dot(norm, hw_dir), 0.0), m_shine);
    vec3 specular = (spec * m_spec) * l_col;

    vec3 b_color = use_tex ? texture(texture_diffuse, f_tex).rgb : vec3(1.0);

    vec3 final_col = (ambient + diffuse) * b_color + specular;

    f_color = vec4(final_col, 1.0);
}