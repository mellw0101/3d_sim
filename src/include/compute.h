#pragma once

#include <GL/glew.h>
#include <Mlib/Vector.h>
#include <glm/ext/vector_float3.hpp>

namespace /* Defines */ {
  #define FPS 120
  #define FRAMETIME_S (1.0f / FPS)

  #define __INLINE_NAMESPACE(name) \
    __inline__ namespace name

  #define __SHADER_DATA_STRUCT(name, ...) \
    typedef struct alignas(16) name { \
      __VA_ARGS__ \
    } name;
}

__INLINE_NAMESPACE(ComputeStructs) {
  __SHADER_DATA_STRUCT(vec3f,
    float x;
    float y;
    float z;

    operator glm::vec3(void) const {
      return {this->x, this->y, this->z};
    }

    vec3f &operator=(const glm::vec3 &vec) {
      this->x = vec.x;
      this->y = vec.y;
      this->z = vec.z;
      return *this;
    }
  )

  struct Particle {
    vec3f pos;
    vec3f vel;
    vec3f accel;
  };

  struct ComputeData {
    vec3f pos;
    vec3f vel;
    vec3f accel;
    vec3f size;
    int flags[2];
  };
}

enum OperationType {
  GRAVITY_OPERATION,
  COLLISION_OPERATION
};

class ComputeObject {
 private:
  int dt_loc;
  int f_loc;
  int operation_loc;

 public:
  Uint operation;
  Uint ssbo;
  Uint program;
  MVector<ComputeData> data;

  ~ComputeObject(void) {
    glDeleteBuffers(1, &ssbo);
    glDeleteProgram(program);
  }

  void init(Uint program, Uint num) {
    for (Uint i = 0; i < num; ++i) {
      data.push_back({});
    }
    this->program = program;
    glGenBuffers(1, &ssbo);
    glUseProgram(program);
    // Set Uniforms.
    dt_loc        = glGetUniformLocation(program, "delta_t");
    f_loc         = glGetUniformLocation(program, "c_force");
    operation_loc = glGetUniformLocation(program, "operation");
    glUniform1f(dt_loc, FRAMETIME_S);
    glUniform3f(f_loc, 0.0f, -9.806f, 0.0f);
  }

  void perform(Uint operation) {
    glUseProgram(program);
    glUniform1ui(operation_loc, operation);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    // Input data into shader buffer.
    glBufferData(GL_SHADER_STORAGE_BUFFER, (data.size() * sizeof(ComputeData)), data.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
    // Dispatch compute shader.
    glDispatchCompute(data.size(), 1, 1);
    // Ensure completion before accessing buffer data.
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    ComputeData *ptr = (ComputeData *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    for (Uint i = 0; i < data.size(); ++i) {
      data[i] = ptr[i];
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }
};