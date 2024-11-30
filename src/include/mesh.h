#pragma once

/* clang-format off */

#include "def.h"
#include "utils.h"

class Mesh;

__INLINE_NAMESPACE(MeshTools) {
  static __INLINE_CONSTEXPR_VOID check_camera_collision(CameraObject *camera, Mesh *mesh);
}

/* Mesh flags. */
#define MESH_FLAGS(mesh, flag)   (mesh)->flags[flag / 32]
#define MESH_FLAGPOS(mesh, flag) (1 << (flag % 32))
#define MESH_ISSET(mesh, flag)   (MESH_FLAGS(mesh, flag) & MESH_FLAGPOS(mesh, flag))
#define MESH_SET(mesh, flag)     MESH_FLAGS(mesh, flag) |= MESH_FLAGPOS(mesh, flag)
#define MESH_UNSET(mesh, flag)   MESH_FLAGS(mesh, flag) &= ~MESH_FLAGPOS(mesh, flag)
#define MESH_TOGGLE(mesh, flag)  MESH_FLAGS(mesh, flag) ^= MESH_FLAGPOS(mesh, flag)

#define STATIC_MESH 1

class Mesh {
 private:
  Uint VAO;
  Uint VBO;
  Uint EBO;
  Uint indices_count;
  int color_loc;
  int model_loc;
  int view_loc;
  int projection_loc;
  int expansion_factor_loc;
  int scale_loc;
  int rotation_loc;
  int pos_loc;

 public:
  int flags[2];

  Uint shader_program;
  mat4 model;
  vec3 color;
  float expansion_factor;
  vec3 pos;
  vec3 vel;
  vec3 accel;
  vec3 rotation;
  vec3 size;
  vec3 _scale;

  Mesh(const MVector<float> &verts,
       const MVector<Uint> &indices,
       Uint shader_program,
       const vec3 &color = {1.0f, 0.5f, 0.2f} /* Default to orange color. */,
       const vec3 &pos = {},
       const vec3 &vel = {},
       const vec3 &rotation = {},
       float expansion = 0.0f)
    :
    indices_count(indices.size()),
    shader_program(shader_program),
    model(1.0f),
    color(color),
    expansion_factor(expansion),
    pos(pos),
    vel(0.0f),
    rotation(0.0f),
    size(verts_size_vec(verts)),
    _scale(1.0f)
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    /* Set up VBO. */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    /* Set up EBO. */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Uint), indices.data(), GL_STATIC_DRAW);
    /* Set up vertex attrribute pointers. */
    /* Position. */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    /* Normal. */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    /* Unbind VAO. */
    glBindVertexArray(0);
    /* Retrieve color uniform from shader. */
    color_loc      = glGetUniformLocation(shader_program, "input_color");
    /* Retrive uniforms from shader. */
    model_loc      = glGetUniformLocation(shader_program, "model");
    view_loc       = glGetUniformLocation(shader_program, "view");
    projection_loc = glGetUniformLocation(shader_program, "projection");
    /* Retrive expansion factor. */
    expansion_factor_loc = glGetUniformLocation(shader_program, "expansion_factor");
    scale_loc    = glGetUniformLocation(shader_program, "scale");
    rotation_loc = glGetUniformLocation(shader_program, "rotation");
    pos_loc      = glGetUniformLocation(shader_program, "pos");
  }

  ~Mesh(void) {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  void set_model_matrix(const mat4 &matrix) {
    model = matrix;
  }

  void draw(GameObject *game) {
    set_sun_direction(game, this->pos);
    set_sun_light_uniforms(game);
    check_camera_collision(&game->camera, this);
    glUseProgram(shader_program);
    glUniform3fv(rotation_loc, 1, &rotation[0]);
    glUniform3fv(pos_loc, 1, &pos[0]);
    /* Pass color to shader. */
    glUniform3fv(color_loc, 1, &color[0]);
    /* Pass _scale vec3 to the shader. */
    glUniform3fv(scale_loc, 1, &_scale[0]);
    /* Pass matrices to shader. */
    model = mat4(1.0f);
    model = scale_matrix(model, _scale);
    model = translate_matrix(model, pos);
    glUniformMatrix4fv(model_loc,      1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(view_loc,       1, GL_FALSE, &game->camera.view[0][0]);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &game->projection[0][0]);
    /* Pass expansion factor to shader */
    glUniform1f(expansion_factor_loc, expansion_factor);
    /* Draw the mesh. */
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  ComputeData compute_data(void) const {
    ComputeData cd;
    cd.pos   = this->pos;
    cd.vel   = this->vel;
    cd.accel = this->accel;
    cd.size  = this->size;
    cd.flags[0] = this->flags[0];
    cd.flags[1] = this->flags[1];
    return cd;
  }

  void compute_data(ComputeData *data) {
    this->pos = data->pos;
    this->vel = data->vel;
    this->accel = data->accel;
    this->size = data->size;
    this->flags[0] = data->flags[0];
    this->flags[1] = data->flags[1];
  }
};

__INLINE_NAMESPACE(MeshTools) {
  // __INLINE_CONSTEXPR_VOID update_mesh_rotation(Mesh *mesh) {
  //   /* Apply rotation around each axis */
  //   mesh->model = glm::rotate(mesh->model, radiansf(mesh->rotation.x), /* X-axis */ vec3(1.0f, 0.0f, 0.0f));  
  //   mesh->model = glm::rotate(mesh->model, radiansf(mesh->rotation.y), /* Y-axis */ vec3(0.0f, 1.0f, 0.0f));  
  //   mesh->model = glm::rotate(mesh->model, radiansf(mesh->rotation.z), /* Z-axis */ vec3(0.0f, 0.0f, 1.0f));
  // }

  __INLINE_CONSTEXPR_VOID accelerate_mesh(Mesh *mesh, const vec3 &f, float dt) {
    rk4_step(&mesh->pos, &mesh->vel, dt, f);
  }

  __INLINE_CONSTEXPR_VOID apply_gravity(Mesh *mesh, float delta_t) {
    accelerate_mesh(mesh, GRAVITY_FORCE, delta_t);
    /* Collision check with ground (if the object hits the ground) */
    if (mesh->pos.y < 0.0f) {
      mesh->pos.y = 0.0f;
      /* Stop downward velocity */
      mesh->vel.y = 0.0f;
    }
  }

  __INLINE_CONSTEXPR_VOID draw_mesh(GameObject *game, Mesh *mesh) {
    mesh->draw(game);
  }

  __INLINE_CONSTEXPR_VOID change_mesh_rotation(Mesh *mesh, const vec3 &rotation) {
    mesh->rotation += rotation;
  }

  __INLINE_CONSTEXPR_VOID set_mesh_rotation(Mesh *mesh, const vec3 &roatation) {
    mesh->rotation = roatation;
  }

  inline void set_mesh_pos(Mesh *mesh, const vec3 &pos) {
    mesh->pos = pos;
    mesh->model = translate_matrix({1.0f}, mesh->pos);
  }

  inline void change_mesh_pos(Mesh *mesh, const vec3 &change) {
    mesh->pos += change;
    mesh->model = translate_matrix({1.0f}, mesh->pos);
  }

  inline void print_mesh_pos(Mesh *mesh) {
    vec3 pos = mat_position_vec(mesh->model);
    printf("pos_x: %f, pos_y: %f, pos_z: %f\n", pos.x, pos.y, pos.z);
  }

  __INLINE_CONSTEXPR_VOID print_mesh_size(Mesh *mesh) {
    printf("pos_x: %f, pos_y: %f, pos_z: %f\n",
      (mesh->size.x * mesh->_scale.x), (mesh->size.y * mesh->_scale.y), (mesh->size.z * mesh->_scale.z));
  }

  __INLINE_CONSTEXPR_VOID mesh_collison_check(Mesh *m1, Mesh *m2) {
    if (MESH_COLLIDING(m1, m2)) {
      if (MESH_OVERLAP_LEAST_L(m1, m2)) {
      }
      else if (MESH_OVERLAP_LEAST_R(m1, m2)) {
      }
      else if (MESH_OVERLAP_LEAST_T(m1, m2)) {
      }
      else if (MESH_OVERLAP_LEAST_B(m1, m2)) {
        m1->pos.y = (MESH_B(m2) + (m1->size.y / 2));
        m1->vel.y = 0;
      }
      else if (MESH_OVERLAP_LEAST_F(m1, m2)) {

      }
      else if (MESH_OVERLAP_LEAST_BK(m1, m2)) {

      }
    }
  }

  __INLINE_CONSTEXPR_VOID check_camera_collision(CameraObject *camera, Mesh *mesh) {
    if (MESH_COLLIDING(camera, mesh)) {
      if (MESH_OVERLAP_LEAST_L(camera, mesh)) {
        camera->pos.x = (MESH_L(mesh) - (camera->size.x / 2));
        camera->pos.x = 0.0f;
      }
      else if (MESH_OVERLAP_LEAST_R(camera, mesh)) {
        camera->pos.x = (MESH_R(mesh) + (camera->size.x / 2));
        camera->vel.x = 0.0f;
      }
      else if (MESH_OVERLAP_LEAST_T(camera, mesh)) {
        camera->pos.y = (MESH_T(mesh) - (camera->size.y / 2));
        camera->vel.y = 0.0f;
      }
      else if (MESH_OVERLAP_LEAST_B(camera, mesh)) {
        camera->pos.y = (MESH_B(mesh) + (camera->size.y / 2));
        camera->vel.y = 0.0f;
      }
      else if (MESH_OVERLAP_LEAST_F(camera, mesh)) {
        camera->pos.z = (MESH_F(mesh) - (camera->size.z / 2));
        camera->vel.z = 0.0f;
      }
      else if (MESH_OVERLAP_LEAST_BK(camera, mesh)) {
        camera->pos.z = (MESH_BK(mesh) + (camera->size.z / 2));
        camera->vel.z = 0.0f;
      }
    }
  }
}

__NAMESPACE(MeshObject) {
  __INLINE_NAMESPACE(ShapeData) {
    // Vertices for a cube (24 vertices, each with a unique position and normal)
    static const MVector<float> cube_vertices = {
      // Positions            // Normals

      // Back face
      -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
      0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
      0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
      -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

      // Front face
      -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
      0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
      -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,

      // Left face
      -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
      -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
      -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
      -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

      // Right face
      0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
      0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
      0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

      // Bottom face
      -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
      -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
      0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,

      // Top face
      -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
      0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f
    };

    // Indices for each face (36 indices total)
    static const MVector<Uint> cube_indices = {
      0, 1, 2, 2, 3, 0,   // Back face
      4, 5, 6, 6, 7, 4,   // Front face
      8, 9, 10, 10, 11, 8,   // Left face
      12, 13, 14, 14, 15, 12,   // Right face
      16, 17, 18, 18, 19, 16,   // Bottom face
      20, 21, 22, 22, 23, 20    // Top face
    };
  }

  class Stairs {
   private:
    MVector<Mesh> data;

   public:
    Stairs(Uint shader, const vec3 &pos, Uint steps, const vec3 &step_size, const vec3 &color) {
      for (Uint i = 0; i < steps; ++i) {
        Mesh step(cube_vertices, cube_indices, shader);
        step.pos.x = pos.x;
        step.pos.y = (pos.y + (i * step_size.y));
        step.pos.z = (pos.z + (i * step_size.z));
        step._scale = step_size;
        step.color = color;
        data.push_back(step);
      }
    }

    void draw(GameObject *game) {
      for (auto &step : data) {
        step.draw(game);
      }
    }
  };
}