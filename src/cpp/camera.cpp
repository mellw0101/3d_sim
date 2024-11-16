#include "../include/prototypes.h"

void init_camera(CameraObject *camera, const glm::mat4 &m, const vec3 &v) {
  camera->size = vec3(1.0f, 2.0f, 1.0f);
  camera->pos = v;
  camera->accel = {0.0f, 0.0f, 0.0f};
  camera->vel = {0.0f, 0.0f, 0.0f};
  camera->view = glm::translate(m, camera->pos);
}

/* Calculate the camera's forward direction from yaw and pitch */
void get_camera_direction(CameraObject *camera) {
  camera->direction = glm::normalize(direction_vec(camera->yaw, camera->pitch));
  camera->up = {0.0f, 1.0f, 0.0f};
  camera->flag.unset<CAMERA_ANGLE_CHANGED>();
}

void calculate_yaw_pitch_from_direction(CameraObject *camera, const vec3 &direction) {
  /* Ensure the direction is normalized */
  glm::vec3 dir = glm::normalize(direction);
  /* Calculate pitch and yaw from the direction vector */
  camera->yaw   = degreesf(atan2(dir.z, dir.x)); 
  camera->pitch = degreesf(asin(dir.y));
}

void change_camera_angle(CameraObject *camera, const vec2 &change) {
  camera->flag.set<CAMERA_ANGLE_CHANGED>();
  camera->yaw   += (change.x * camera->sensitivity);
  camera->pitch += (change.y * camera->sensitivity);
  Clamp(camera->pitch, -89.9f, 89.9f);
}

void set_camera_view(CameraObject *camera, const vec3 &pos) {
  camera->pos = pos;
  camera->view = glm::translate({1.0f}, camera->pos);
}

void change_camera_pos(CameraObject *camera, const vec3 &change) {
  /* Calculate forward, right, and up vectors based on yaw and pitch */
  vec3 forward;
  vec3 right = right_vec(camera->yaw);
  vec3 up    = {0.0f, 1.0f, 0.0f};
  if (camera->flag.is_set<FreeCamera>()) {
    forward = direction_vec(camera->yaw, camera->pitch);
  }
  else {
    forward = direction_vec(camera->yaw, 0.0f);
    /* Nullify any up/down movement. */
    forward.y = 0.0f;
    right.y   = 0.0f;
  }
  /* Move the camera based on the relative change. */
  camera->pos -= (forward * change.z) + (right * change.x) + (up * change.y);
}

void update_camera(CameraObject *camera) {
  rk4_step(&camera->pos, &camera->vel, FRAMETIME_S, (camera->accel + GRAVITY_FORCE));
  if (camera->pos.y < 0.0f) {
    camera->pos.y = 0.0f;
    camera->vel.y = 0.0f;
  }
  camera->accel = {0.0f, 0.0f, 0.0f};
  if (camera->flag.is_set<CAMERA_ANGLE_CHANGED>()) {
    get_camera_direction(camera);
  }
  camera->view = glm::lookAt(camera->pos, (camera->pos - camera->direction), camera->up);
}


