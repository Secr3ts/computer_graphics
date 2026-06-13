#version 420 core
out vec4 f_color;

in vec2 tex_coords;
uniform sampler2D screen_tex;
uniform sampler2D noise_tex;

uniform float intensity;
uniform float noise_mix;

void main() {
    vec4 color = texture(screen_tex, tex_coords);
    vec3 rgb = color.rgb;
    
    // Apply sepia tone using standard sepia matrix
    vec3 sepia = vec3(
        dot(rgb, vec3(0.393, 0.769, 0.189)),
        dot(rgb, vec3(0.349, 0.686, 0.168)),
        dot(rgb, vec3(0.272, 0.534, 0.131))
    );
    
    // Mix original color with sepia based on intensity
    vec3 final = mix(rgb, sepia, intensity);
    
    vec3 noise = texture(noise_tex, tex_coords).rgb;
    final = mix(final, noise, clamp(noise_mix, 0.0, 1.0));

    f_color = vec4(final, color.a);
}
