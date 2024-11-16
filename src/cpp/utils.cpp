#include "../include/prototypes.h"
#include <glm/ext/matrix_clip_space.hpp>

/* clang-format off */

void init_projection(GameObject *game, float fov, float aspect_ratio, float znear, float zfar) {
  game->projection = glm::perspective(fov, aspect_ratio, znear, zfar);
}

/* Initialize SDL and set all GL attribute pair`s. */
void init_SDL(const MVector<Pair<SDL_GLattr, int>> &gl_attributes) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
    exit(SDL_INIT_ERROR);
  }
  /* Set all attribute pairs. */
  for (const auto &attr : gl_attributes) {
    /* If an attribute failes to be set, print the error, then terminate imedietly. */
    if (SDL_GL_SetAttribute(attr.first, attr.second) != 0) {
      fprintf(stderr, "Failed to set attr: %s\n", SDL_GetError());
      SDL_Quit();
      exit(SDL_SET_ATTR_ERROR);
    }
  }
}

/* Init the game`s SDL window using a title and an int array with size 5.  If title is
 * nullptr "placeholder" will be used, and if data is nullptr some defaults will be used */
void init_SDL_window(GameObject *game, const char *title, int *data) {
  int opt[5];
  /* If data is nullptr, set some defaults. */
  if (!data) {
    opt[0] = SDL_WINDOWPOS_CENTERED;
    opt[1] = SDL_WINDOWPOS_CENTERED;
    opt[2] = 800;
    opt[3] = 600;
    opt[4] = (SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_MAXIMIZED);
  }
  else {
    for (Uint i = 0; i < 5; ++i) {
      opt[i] = data[i];
    }
  }
  /* Create the SDL window. */
  game->win = SDL_CreateWindow(!title ? "placeholder" : title, opt[0], opt[1], opt[2], opt[3], opt[4]);
  /* Terminate apon failure. */
  if (!game->win) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
    SDL_Quit();
    exit(SDL_WINDOW_CREATION_ERROR);
  }
  int w, h;
  SDL_GetWindowSize(game->win, &w, &h);
  game->width  = w;
  game->height = h;
  /* Hides the cursor */
  SDL_ShowCursor(0);
}

void create_SDL_GLContext_and_init_glew(GameObject *game) {
  game->context = SDL_GL_CreateContext(game->win);
  if (!game->context) {
    fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
    SDL_DestroyWindow(game->win);
    SDL_Quit();
    exit(SDL_GLCONTEXT_CREATION_ERROR);
  }
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize glew\n");
    SDL_GL_DeleteContext(game->context);
    SDL_DestroyWindow(game->win);
    SDL_Quit();
    exit(GLEW_INIT_ERROR);
  }
}

/* Cleanup before exit. */
void cleanup(GameObject *game) {
  glDeleteProgram(game->shader_program);
  SDL_GL_DeleteContext(game->context);
  SDL_DestroyWindow(game->win);
  SDL_Quit();
}

/* Calculate the time to sleep based on the duration that has passed sence frame start.  Then sleep. */
void frame_end(time_point<high_resolution_clock> frame_start) {
  time_point now = high_resolution_clock::now();
  duration<double, std::milli> frame_duration = (now - frame_start);
  if (frame_duration.count() >= (1000.0f / FPS)) {
    return;
  }
  duration<double, std::milli> sleep_time((1000.0f / FPS) - frame_duration.count());
  std::this_thread::sleep_for(sleep_time);
}

