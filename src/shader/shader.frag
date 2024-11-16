#version 450 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 input_color;
uniform vec3 view_position;

uniform vec3 sun_pos;
uniform vec3 sun_direction;
uniform vec3 sun_color;
uniform float sun_strength;

float shininess = 32.0;
float specular_strength = 0.1;

vec3 calculate_specular(vec3 normal) {
  // Ensure the view direction is normalized
  vec3 viewDir = normalize(view_position - FragPos);
  // Reflect the light direction around the normal
  vec3 reflectDir = reflect(-sun_direction, normal);
  // Calculate specular factor using the shininess exponent
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  // Calculate final specular color
  vec3 specular = spec * sun_color * sun_strength * specular_strength;
  return specular;
}

void main() {
  vec3 norm = normalize(Normal);
  float diff = max(dot(norm, normalize(-sun_direction)), 0.0);
  vec3 diffuse = diff * sun_color * sun_strength;
  /* Orange color */
  FragColor = vec4(diffuse + calculate_specular(norm) + input_color, 1.0);
}
