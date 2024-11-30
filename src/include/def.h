#pragma once

/* clang-format off */

/* C headers. */
#include <chrono>
#include <stdio.h>
#include <stddef.h>
/* C++ headers. */
#include <fstream>
#include <sstream>
/* Rendering headers. */
#include <SDL2/SDL.h>
#include <GL/glew.h>
/* Includes math struct`s and functions from glm lib. */
#include <glm/glm.hpp>
#include <glm/ext/scalar_uint_sized.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
/* Includes from my custom lib, called Mlib. */
#include <Mlib/Flag.h>
#include <Mlib/float_calc.h>
#include <Mlib/Vector.h>
#include <Mlib/openGL/shader.h>
#include <Mlib/Pair.h>
#include <Mlib/openGL/context.h>
/* Eigen math lib. */
#include <eigen3/Eigen/Geometry>
#include <eigen3/Eigen/Dense>

#include "compute.h"

namespace /* Define. */ {
  #define __INLINE_CONSTEXPR(type) \
    __inline__ constexpr type __attribute__((__const__, __always_inline__, __nodebug__, __nothrow__))

  #define __INLINE_CONSTEXPR_VOID \
    __inline__ constexpr void __attribute__((__always_inline__, __nodebug__, __nothrow__))

  #define __INLINE_NAMESPACE(name) \
    __inline__ namespace name
    
  #define __NAMESPACE(name) \
    namespace name

  #define MESH_L(m)  ((m)->pos.x - ((m)->size.x / 2))
  #define MESH_R(m)  ((m)->pos.x + ((m)->size.x / 2))
  #define MESH_T(m)  ((m)->pos.y - ((m)->size.y / 2))
  #define MESH_B(m)  ((m)->pos.y + ((m)->size.y / 2))
  #define MESH_F(m)  ((m)->pos.z - ((m)->size.z / 2))
  #define MESH_BK(m) ((m)->pos.z + ((m)->size.z / 2))

  #define MESH_COLLIDING(o, so) \
    (MESH_R(o)  >= MESH_L(so) && MESH_L(o) <= MESH_R(so) && \
     MESH_B(o)  >= MESH_T(so) && MESH_T(o) <= MESH_B(so) && \
     MESH_BK(o) >= MESH_F(so) && MESH_F(o) <= MESH_BK(so))

  #define MESH_OVERLAP_L(o, so)  (MESH_R(o)   - MESH_L(so))
  #define MESH_OVERLAP_R(o, so)  (MESH_R(so)  - MESH_L(o))
  #define MESH_OVERLAP_T(o, so)  (MESH_B(o)   - MESH_T(so))
  #define MESH_OVERLAP_B(o, so)  (MESH_B(so)  - MESH_T(o))
  #define MESH_OVERLAP_F(o, so)  (MESH_BK(o)  - MESH_F(so))
  #define MESH_OVERLAP_BK(o, so) (MESH_BK(so) - MESH_F(o))

  #define MESH_OVERLAP_LEAST_L(o, so) \
    (MESH_OVERLAP_L(o, so) < MESH_OVERLAP_R(o, so) && \
     MESH_OVERLAP_L(o, so) < MESH_OVERLAP_T(o, so) && \
     MESH_OVERLAP_L(o, so) < MESH_OVERLAP_B(o, so) && \
     MESH_OVERLAP_L(o, so) < MESH_OVERLAP_F(o, so) && \
     MESH_OVERLAP_L(o, so) < MESH_OVERLAP_BK(o, so))

  #define MESH_OVERLAP_LEAST_R(o, so) \
    (MESH_OVERLAP_R(o, so) < MESH_OVERLAP_L(o, so) && \
     MESH_OVERLAP_R(o, so) < MESH_OVERLAP_T(o, so) && \
     MESH_OVERLAP_R(o, so) < MESH_OVERLAP_B(o, so) && \
     MESH_OVERLAP_R(o, so) < MESH_OVERLAP_F(o, so) && \
     MESH_OVERLAP_R(o, so) < MESH_OVERLAP_BK(o, so))

  #define MESH_OVERLAP_LEAST_T(o, so) \
    (MESH_OVERLAP_T(o, so) < MESH_OVERLAP_B(o, so) && \
     MESH_OVERLAP_T(o, so) < MESH_OVERLAP_L(o, so) && \
     MESH_OVERLAP_T(o, so) < MESH_OVERLAP_R(o, so) && \
     MESH_OVERLAP_T(o, so) < MESH_OVERLAP_F(o, so) && \
     MESH_OVERLAP_T(o, so) < MESH_OVERLAP_BK(o, so))

  #define MESH_OVERLAP_LEAST_B(o, so) \
    (MESH_OVERLAP_B(o, so) < MESH_OVERLAP_T(o, so) && \
     MESH_OVERLAP_B(o, so) < MESH_OVERLAP_L(o, so) && \
     MESH_OVERLAP_B(o, so) < MESH_OVERLAP_R(o, so) && \
     MESH_OVERLAP_B(o, so) < MESH_OVERLAP_F(o, so) && \
     MESH_OVERLAP_B(o, so) < MESH_OVERLAP_BK(o, so))

  #define MESH_OVERLAP_LEAST_F(o, so) \
    (MESH_OVERLAP_F(o, so) < MESH_OVERLAP_BK(o, so) && \
     MESH_OVERLAP_F(o, so) < MESH_OVERLAP_L(o, so) && \
     MESH_OVERLAP_F(o, so) < MESH_OVERLAP_R(o, so) && \
     MESH_OVERLAP_F(o, so) < MESH_OVERLAP_T(o, so) && \
     MESH_OVERLAP_F(o, so) < MESH_OVERLAP_B(o, so))

  #define MESH_OVERLAP_LEAST_BK(o, so) \
    (MESH_OVERLAP_BK(o, so) < MESH_OVERLAP_F(o, so) && \
     MESH_OVERLAP_BK(o, so) < MESH_OVERLAP_L(o, so) && \
     MESH_OVERLAP_BK(o, so) < MESH_OVERLAP_R(o, so) && \
     MESH_OVERLAP_BK(o, so) < MESH_OVERLAP_T(o, so) && \
     MESH_OVERLAP_BK(o, so) < MESH_OVERLAP_B(o, so))
}

using glm::fvec2;
using glm::fvec3;
// using glm::vec2;
// using glm::vec3;
// using glm::vec4;
// using glm::mat4;

using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;
using std::chrono::system_clock;

/* Gravity along Y-axis */
static __inline__ constexpr vec3 GRAVITY_FORCE(0.0f, -GRAVITY, 0.0f);

__INLINE_NAMESPACE(ColorVec) {
  static __inline__ constexpr vec3 red_color_vec = {1.0f, 0.0f, 0.0f};
  static __inline__ constexpr vec3 blue_color_vec = {0.0f, 0.0f, 1.0f};
}

typedef enum : int {
  CLEAN_EXIT,
  SDL_INIT_ERROR,
  SDL_SET_ATTR_ERROR,
  SDL_WINDOW_CREATION_ERROR,
  SDL_GLCONTEXT_CREATION_ERROR,
  GLEW_INIT_ERROR
} ExitStatusCode;

typedef enum {
  FreeCamera,
  CAMERA_ANGLE_CHANGED
} CameraFlag;

typedef struct {
  vec3 pos;
  vec3 vel;
  vec3 accel;
} PositionStruct;

typedef struct {
  float sensitivity;
  float pitch;
  float yaw;
  mat4 view;
  vec3 size;
  vec3 direction;
  vec3 up;
  vec3 pos;
  vec3 vel;
  vec3 accel;
  bit_flag_t<8> flag;
} CameraObject;

typedef enum {
  RUNNING
} GameObjectState;

class SunLightObject {
 private:
  // locs.
  int dir_loc;
  int color_loc;
  int strength_loc;

 public:
  Uint shader;
  vec3 pos;
  vec3 direction;
  vec3 color;
  float strength;

  void init_loc(void) {
    glUseProgram(shader);
    dir_loc      = glGetUniformLocation(shader, "sun_direction");
    color_loc    = glGetUniformLocation(shader, "sun_color");
    strength_loc = glGetUniformLocation(shader, "sun_strength");
  }

  void set_uniforms(void) {
    glUseProgram(shader);
    glUniform3fv(dir_loc, 1, &direction[0]);
    glUniform3fv(color_loc, 1, &color[0]);
    glUniform1f(strength_loc, strength);
  }
};

typedef struct {
  /* OpenGL data. */
  Uint shader_program;
  Uint compute_program;
  int view_loc;
  int proj_loc;
  /* Camera data. */
  mat4 projection;
  CameraObject camera;
  SunLightObject sun;
  /* Game window size. */
  float width;
  float height;
  /* Game state. */
  bit_flag_t<8> state;
  /* SDL data. */
  SDL_Event ev;
  SDL_Window *win;
  SDL_GLContext context;
  // Types.
  ComputeObject compute;
} GameObject;