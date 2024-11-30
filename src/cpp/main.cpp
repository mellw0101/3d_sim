#include "../include/prototypes.h"

void prosses_held_keys(GameObject *game) {
  const Uchar *state = SDL_GetKeyboardState(nullptr);
  /* For now exit on esc. */
  if (state[SDL_SCANCODE_ESCAPE]) {
    game->state.unset<RUNNING>();
    return;
  }
  if (state[SDL_SCANCODE_S]) {
    change_camera_pos(&game->camera, {0.0f, 0.0f, -0.02f});
  }
  if (state[SDL_SCANCODE_W]) {
    change_camera_pos(&game->camera, {0.0f, 0.0f, 0.02f});
  }
  if (state[SDL_SCANCODE_D]) {
    change_camera_pos(&game->camera, {0.02f, 0.0f, 0.0f});
  }
  if (state[SDL_SCANCODE_A]) {
    change_camera_pos(&game->camera, {-0.02f, 0.0f, 0.0f});
  }
  if (state[SDL_SCANCODE_SPACE]) {
    game->camera.vel.y = 5.0f;
  }
}

void handle_events(GameObject *game) {
  while (SDL_PollEvent(&game->ev)) {
    switch (game->ev.type) {
    case SDL_QUIT:
      game->state.unset<RUNNING>();
      break;
    case SDL_MOUSEMOTION:
      SDL_WarpMouseInWindow(game->win, (game->width / 2), (game->height / 2));
      change_camera_angle(&game->camera, {(float)game->ev.motion.xrel, (float)game->ev.motion.yrel});
      break;
    }
  }
}

/* clang-format off */
inline namespace Shapes {
  static MVector<float> floor_vertices = {
    /* positions */       /* normals */
    -5.0f, 0.0f, -5.0f,   0.0f, 1.0f, 0.0f, /* Bottom-left */
     5.0f, 0.0f, -5.0f,   0.0f, 1.0f, 0.0f, /* Bottom-right */
     5.0f, 0.0f,  5.0f,   0.0f, 1.0f, 0.0f, /* Top-right */
    -5.0f, 0.0f,  5.0f,   0.0f, 1.0f, 0.0f  /* Top-left */
  };

  static MVector<Uint> floor_indices = {
    0, 1, 2, /* First triangle */
    0, 2, 3  /* Second triangle */
  };

  /* Define vertices and indices for a simple triangle. */
  MVector<float> triangleVertices = {
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, /* Bottom-left */
    0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, /* Bottom-right */
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  /* Top-center */
  };
  MVector<Uint> triangleIndices = {0, 1, 2};

  /* Vertices for a cube (24 vertices, each with a unique position and normal) */
  static const MVector<float> cubeVertices = {
    /* Positions */        /* Normals */
    /* Back face */
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
    /* Front face */
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
    /* Left face */
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
    /* Right face */
    0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,
    /* Bottom face */
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
    /* Top face */
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f
  };

  // Indices for each face (36 indices total)
  static const MVector<Uint> cubeIndices = {
    0,  1,  2,  2,  3,  0,  // Back face
    4,  5,  6,  6,  7,  4,  // Front face
    8,  9,  10, 10, 11, 8,  // Left face
    12, 13, 14, 14, 15, 12, // Right face
    16, 17, 18, 18, 19, 16, // Bottom face
    20, 21, 22, 22, 23, 20  // Top face
  };
}

void test_mat4(void) {
  mat4 a(
    1.0,  2.0,  3.0,  4.0,
    5.0,  6.0,  7.0,  8.0,
    9.0, 10.0, 11.0, 12.0,
    13.0, 14.0, 15.0, 16.0
  );
  mat4 b(
    16.0 ,15.0 ,14.0, 13.0,
    12.0 ,11.0 ,10.0 , 9.0,
    8.0  ,7.0  ,6.0 , 5.0,
    4.0  ,3.0  ,2.0 , 1.0
  );
  mat4 r = (a * b);
  for (Uint i = 0; i < 4; ++i) {
    printf("[ ");
    for (Uint j = 0; j < 4; ++j) {
      if (j != 3) {
        printf("%f, ", r[i][j]);
      }
      else {
        printf("%f", r[i][j]);
      }
    }
    printf(" ]\n");
  }
  fflush(stdout);
}

int main(void) {
  GameObject game;
  game.camera.sensitivity = 0.07f;
  // calculate_yaw_pitch_from_direction(&game.camera, {0.0f, 0.0f, -3.0f});
  yaw_pitch_from_direction(vec3(0.0f, 0.0f, -3.0f), &game.camera.yaw, &game.camera.pitch);
  /* Init SDL. */
  openGL_init_SDL_with_GL_attr({
    {SDL_GL_CONTEXT_MAJOR_VERSION, 4},
    {SDL_GL_CONTEXT_MINOR_VERSION, 5},
    {SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE}
  });
  /* Create the window. */
  init_SDL_window(&game);
  game.context = openGL_init_SDL_GLContext(game.win, true);
  /* Create an OpenGL context. */
  /* Setup view-port. */
  glViewport(0, 0, game.width, game.height);
  /* Load and compile shaders. */
  game.shader_program = create_shader_program({
    {"src/shader/shader.vert", GL_VERTEX_SHADER},
    {"src/shader/shader.frag", GL_FRAGMENT_SHADER},
    }, {}
  );
  game.compute.init(create_comp_shader_program("src/shader/shader.comp"), 2);
  init_camera(&game.camera);
  init_projection(&game, radiansf(80.0f), (game.width / game.height), 0.1f, 100.0f);
  /* Create a Mesh object for the triangle */
  game.sun.pos = {0.0f, 20.0f, 0.0f};
  set_sun_light(&game, direction_vec(vec3(0.0f), game.sun.pos), {1.0f, 1.0f, 1.0f}, 0.4f);
  set_sun_light_uniforms(&game);
  Mesh triangle(triangleVertices, triangleIndices, game.shader_program, red_color_vec);
  Mesh floor(floor_vertices, floor_indices, game.shader_program);
  floor.pos.y = -2.0f;
  Mesh cube(cubeVertices, cubeIndices, game.shader_program, red_color_vec);
  Mesh cube2(cubeVertices, cubeIndices, game.shader_program, blue_color_vec);
  MESH_SET(&cube2, STATIC_MESH);
  cube.pos.y = 4.0f;
  cube._scale.x = 2.0f;
  game.state.set<RUNNING>();
  MVector<Mesh> meshes;
  meshes.push_back(cube);
  meshes.push_back(cube2);
  /* Main loop. */
  while (game.state.is_set<RUNNING>()) {
    time_point frame_start = high_resolution_clock::now();
    prosses_held_keys(&game);
    handle_events(&game);
    glClear(GL_COLOR_BUFFER_BIT);
    update_camera(&game.camera);
    /* Draw floor first. */
    draw_mesh(&game, &floor);
    /* Draw the triangle. */
    draw_mesh(&game, &triangle);
    check_camera_collision(&game.camera, &cube2);
    for (Uint i = 0; i < 2; ++i) {
      game.compute.data[i] = meshes[i].compute_data();
    }
    game.compute.perform(GRAVITY_OPERATION);
    for (Uint i = 0; i < 2; ++i) {
      meshes[i].compute_data(&game.compute.data[i]);
    }
    // apply_gravity(&cube, FRAMETIME_S);
    for (Uint i = 0; i < 2; ++i) {
      game.compute.data[i] = meshes[i].compute_data();
    }
    game.compute.perform(COLLISION_OPERATION);
    for (Uint i = 0; i < 2; ++i) {
      meshes[i].compute_data(&game.compute.data[i]);
      draw_mesh(&game, &meshes[i]);
    }
    printf("pos.y: %f, vel.y: %f\n", game.compute.data[0].pos.y, game.compute.data[0].vel.y);
    // mesh_collison_check(&cube, &cube2);
    /* Swap buffers. */
    SDL_GL_SwapWindow(game.win);
    frame_end(frame_start);
  }
  /* Cleanup. */
  cleanup(&game);
  exit(CLEAN_EXIT);
}
