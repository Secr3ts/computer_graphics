#version 420 core
out vec4 f_color;

in vec3 f_pos;
in vec3 f_nor;
in vec2 f_tex;

in vec3 m_amb;   // HARDWARE INSTANCING
in vec3 m_dif;// HARDWARE INSTANCING
in vec3 m_spec;// HARDWARE INSTANCING
in float m_shine;// HARDWARE INSTANCING

uniform vec3 l_pos;
uniform vec3 v_pos;
uniform vec3 l_col;

uniform sampler2D texture_diffuse;
uniform bool use_tex;

void main() {
    vec3 sky_color = vec3(0.6, 0.8, 1.0);
    vec3 ground_color = vec3(0.2, 0.2, 0.2);

    vec3 norm = normalize(f_nor);
    
    float mix_factor = 0.5 * (norm.y + 1.0); 
    vec3 indirect_diffuse = mix(ground_color, sky_color, mix_factor) * m_amb;

    vec3 l_dir = normalize(l_pos - f_pos);
    float diff = max(dot(norm, l_dir), 0.0);
    vec3 direct_diffuse = (diff * m_dif) * l_col;

    vec3 v_dir = normalize(v_pos - f_pos);
    vec3 hw_dir = normalize(l_dir + v_dir);
    float spec = pow(max(dot(norm, hw_dir), 0.0), m_shine);
    vec3 direct_specular = (spec * m_spec) * l_col;

    vec3 b_color = use_tex ? texture(texture_diffuse, f_tex).rgb : vec3(1.0);

    vec3 final_col = (indirect_diffuse + direct_diffuse) * b_color + direct_specular;

    f_color = vec4(final_col, 1.0);
}