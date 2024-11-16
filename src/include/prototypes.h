#pragma once

/* clang-format off */

#include "mesh.h"
#include "def.h"
#include "utils.h"

/* shader.cpp */
Uint create_shader_program(const MVector<Pair<const char *, Uint>> &parts, const MVector<const char *> &includes);
Uint create_comp_shader_program(const char *path);

/* utils.cpp */
void set_correct_view_direction(GameObject *game);
void init_projection(GameObject *game, float fov, float aspect_ratio, float znear, float zfar);
void init_SDL(const MVector<Pair<SDL_GLattr, int>> &gl_attributes);
void init_SDL_window(GameObject *game, const char *title = nullptr, int *data = nullptr);
void create_SDL_GLContext_and_init_glew(GameObject *game);
void cleanup(GameObject *game);
void frame_end(time_point<high_resolution_clock> frame_start);

/* camera.cpp */
void init_camera(CameraObject *camera, const glm::mat4 &m = {1.0f}, const vec3 &v = {0.0f, 0.0f, -3.0f}) ;
void get_camera_direction(CameraObject *camera);
void calculate_yaw_pitch_from_direction(CameraObject *camera, const glm::vec3 &direction);
void change_camera_angle(CameraObject *camera, const glm::vec2 &change);
void set_camera_pos(CameraObject *camera, const vec3 &pos);
void change_camera_pos(CameraObject *camera, const glm::vec3 &change);
void update_camera(CameraObject *camera);