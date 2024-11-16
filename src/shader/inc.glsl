#version 450 core
/* This is a global header for the shaders.  This is
 * done by adding this infront of all shaders loaded */

/* Flags. */
int flags[2] = int[2](0, 0);
#define FLAGS(flag)   flags(flag / 32)
#define FLAGPOS(flag) (1 << (flag % 32))
#define ISSET(flag)   (FLAGS(flag) & FLAGPOS(flag))
#define SET(flag)     FLAGS(flag) |= FLAGPOS(flag)
#define UNSET(flag)   FLAGS(flag) &= ~FLAGPOS(flag)
#define TOGGLE(flag)  FLAGS(flag) ^= FLAGPOS(flag)

mat4 rotation_matrix_x(float angle) {
  float c = cos(angle);
  float s = sin(angle);
  return mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, c,   -s,  0.0,
    0.0, s,   c,   0.0,
    0.0, 0.0, 0.0, 1.0
  );
}

mat4 rotation_matrix_y(float angle) {
  float c = cos(angle);
  float s = sin(angle);
  return mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, c,   -s,  0.0,
    0.0, s,   c,   0.0,
    0.0, 0.0, 0.0, 1.0
  );
}

mat4 rotation_matrix_z(float angle) {
  float c = cos(angle);
  float s = sin(angle);
  return mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, c,   -s,  0.0,
    0.0, s,   c,   0.0,
    0.0, 0.0, 0.0, 1.0
  );
}

mat4 rotation_matrix(vec3 rotation) {
  return (rotation_matrix_z(rotation.z) * rotation_matrix_y(rotation.y) * rotation_matrix_x(rotation.x));
}

vec3 direction_to(vec3 to, vec3 from) {
  return normalize(to - from);
}