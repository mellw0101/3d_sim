#version 450 core

layout(location = 0) in vec3 aPos;    /* Vertex position. */
layout(location = 1) in vec3 aNormal; /* Vertex normal. */

out vec3 FragPos; /* Position of the fragment. */
out vec3 Normal;  /* Normal of the fragment. */

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 scale;
uniform vec3 rotation;
uniform vec3 pos;

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

mat4 scale_matrix(vec3 scale) {
  return mat4(
    scale.x, 0.0,     0.0,     0.0,
    0.0,     scale.y, 0.0,     0.0,
    0.0,     0.0,     scale.z, 0.0,
    0.0,     0.0,     0.0,     1.0
  );
}

mat4 translate(mat4 matrix, vec3 pos) {
  matrix[3][0] += pos.x;
  matrix[3][1] += pos.y;
  matrix[3][2] += pos.z;
  return matrix;
}

mat4 identity(float val) {
  return mat4(
    val,  0.0f, 0.0f, 0.0f,
    0.0f, val,  0.0f, 0.0f,
    0.0f, 0.0f, val,  0.0f,
    0.0f, 0.0f, 0.0f, val
  );
}

mat4 create_model_matrix(vec3 scale, vec3 rotation, vec3 pos) {
  mat4 smat = scale_matrix(scale);
  mat4 rmat = rotation_matrix(rotation);
  mat4 tmat = identity(1.0f);
  mat4 ret = tmat * rmat * smat;
  ret = translate(ret, pos);
  return ret;
}

void main() {
  // mat4 model = create_model_matrix(scale, rotation, pos);
  /* Calculate the vertex position in world space. */
  FragPos = vec3(model * vec4(aPos, 1.0));
  /* Transform the normal vector by the invers transpose of the model matrix. */
  Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
  /* Calculate the final position. */
  gl_Position = projection * view * vec4(FragPos, 1.0);
}
