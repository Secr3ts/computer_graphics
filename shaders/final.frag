#version 430

in vec3 v_Normal;
in vec3 v_WorldPos;
in vec2 v_Texcoords;

out vec4 fragColor;

const vec3 LightDirection = vec3(1.0, -1.0, -1.0);
const vec3 LightColor = vec3(1.0, 1.0, 1.0);
const vec3 Kd = vec3(1.0, 1.0, 1.0);
const vec3 Ks = vec3(0.35, 0.35, 0.35);
const float Shininess = 32.0 * 3;

uniform vec3 u_viewPos;
uniform sampler2D u_sampler;

vec3 diffuse(vec3 N, vec3 L) {
  float lambert = max(dot(normalize(N), normalize(L)), 0.0);
  return lambert * LightColor * Kd;
}

vec3 specular(vec3 N, vec3 L, vec3 V) {
  vec3 H = normalize(normalize(L) + normalize(V));
  float highlight = pow(max(dot(normalize(N), H), 0.0), Shininess);
  return highlight * LightColor * Ks;
}

void main() {
  vec3 N = normalize(v_Normal);
  vec3 L = normalize(-LightDirection);
  vec3 V = normalize(u_viewPos - v_WorldPos);
  vec4 texColor = texture(u_sampler, v_Texcoords);
  vec3 color = (diffuse(N, L) + specular(N, L, V)) * texColor.rgb;
  fragColor = vec4(color, texColor.a);
}