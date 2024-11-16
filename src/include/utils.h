#pragma once
/* clang-format off */

#include "def.h"
#include <Mlib/float_calc.h>

__INLINE_CONSTEXPR(vec3) direction_vec(float yaw, float pitch) {
  vec3 dir;
  dir.x = (cos(radiansf(yaw)) * cos(radiansf(pitch)));
  dir.y = sin(radiansf(pitch));
  dir.z = (sin(radiansf(yaw)) * cos(radiansf(pitch)));
  return dir;
}

__INLINE_CONSTEXPR(vec3) direction_vec(const vec3 &to, const vec3 &from) {
  return glm::normalize(to - from);
}

__INLINE_CONSTEXPR(vec3) right_vec(float yaw) {
  vec3 right;
  right.x = cos(radiansf(yaw + 90.0f));
  right.y = 0.0f;
  right.z = sin(radiansf(yaw + 90.0f));
  return right;
}

__INLINE_CONSTEXPR_VOID rk4_step(vec3 *pos, vec3 *vel, float dt, const vec3 &f) {
  /* K1. */
  float k1_x  = (vel->x * dt);
  float k1_y  = (vel->y * dt);
  float k1_z  = (vel->z * dt);
  float k1_vx = (f.x * dt);
  float k1_vy = (f.y * dt);
  float k1_vz = (f.z * dt);
  /* K2. */
  float k2_x  = ((vel->x + 0.5f * k1_vx) * dt);
  float k2_y  = ((vel->y + 0.5f * k1_vy) * dt);
  float k2_z  = ((vel->z + 0.5f * k1_vz) * dt);
  float k2_vx = (f.x * dt);
  float k2_vy = (f.y * dt);
  float k2_vz = (f.z * dt);
  /* K3. */
  float k3_x  = ((vel->x + 0.5f * k2_vx) * dt);
  float k3_y  = ((vel->y + 0.5f * k2_vy) * dt);
  float k3_z  = ((vel->z + 0.5f * k2_vz) * dt);
  float k3_vx = f.x * dt;
  float k3_vy = f.y * dt;
  float k3_vz = f.z * dt;
  /* K4. */
  float k4_x  = ((vel->x + k3_vx) * dt);
  float k4_y  = ((vel->y + k3_vy) * dt);
  float k4_z  = ((vel->z + k3_vz) * dt);
  float k4_vx = (f.x * dt);
  float k4_vy = (f.y * dt);
  float k4_vz = (f.z * dt);
  /* Update position. */
  pos->x += ((k1_x + 2.0f * k2_x + 2.0f * k3_x + k4_x) / 6.0f);
  pos->y += ((k1_y + 2.0f * k2_y + 2.0f * k3_y + k4_y) / 6.0f);
  pos->z += ((k1_z + 2.0f * k2_z + 2.0f * k3_z + k4_z) / 6.0f);
  /* Update velocity. */
  vel->x += ((k1_vx + 2.0f * k2_vx + 2.0f * k3_vx + k4_vx) / 6.0f);
  vel->y += ((k1_vy + 2.0f * k2_vy + 2.0f * k3_vy + k4_vy) / 6.0f);
  vel->z += ((k1_vz + 2.0f * k2_vz + 2.0f * k3_vz + k4_vz) / 6.0f);
}

__INLINE_CONSTEXPR(vec3) verts_size_vec(const MVector<float> &verts) {
  vec3 min(verts[0], verts[1], verts[2]);
  vec3 max(verts[0], verts[1], verts[2]);
  for (Uint i = 0; i < verts.size(); i += 6) {
    vec3 vertex(verts[i], verts[i + 1], verts[i + 2]);
    /* Calculate max. */
    min.x = glm::min(min.x, vertex.x);
    min.y = glm::min(min.y, vertex.y);
    min.z = glm::min(min.z, vertex.z);
    /* Calculate max. */
    max.x = glm::max(max.x, vertex.x);
    max.y = glm::max(max.y, vertex.y);
    max.z = glm::max(max.z, vertex.z);
  }
  return (max - min);
}

__INLINE_CONSTEXPR(vec3) mat_scale_vec(const mat4 &mat) {
  vec3 scale;
  scale.x = glm::length(vec3(mat[0]));
  scale.y = glm::length(vec3(mat[1]));
  scale.z = glm::length(vec3(mat[2]));
  return scale;
}

__INLINE_CONSTEXPR(vec3) mat_position_vec(const mat4 &mat) {
  return vec3(mat[3]);
}

__INLINE_CONSTEXPR_VOID set_sun_light(GameObject *game, const vec3 &direction, const vec3 &color, float strength) {
  game->sun.direction = direction;
  game->sun.color = color;
  game->sun.strength = strength;
}

__INLINE_CONSTEXPR_VOID set_sun_direction(GameObject *game, const vec3 &end_pos) {
  vec3 dir = direction_vec(end_pos, game->sun.pos);
  game->sun.direction = dir;
}

inline void set_sun_light_uniforms(GameObject *game) {
  glUseProgram(game->shader_program);
  glUniform3fv(glGetUniformLocation(game->shader_program, "view_position"), 1, &game->camera.pos[0]);
  glUniform3fv(glGetUniformLocation(game->shader_program, "sun_direction"), 1, &game->sun.pos[0]);
  glUniform3fv(glGetUniformLocation(game->shader_program, "sun_direction"), 1, &game->sun.direction[0]);
  glUniform3fv(glGetUniformLocation(game->shader_program, "sun_color"), 1, &game->sun.color[0]);
  glUniform1f(glGetUniformLocation(game->shader_program, "sun_strength"), game->sun.strength);
}