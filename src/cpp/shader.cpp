#include "../include/prototypes.h"

std::string load_shader_source(const char *path) {
  std::ifstream file(path);
  if (file.is_open()) {
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
  }
  return "";
}

std::string load_shader_source_with_includes(const char *path,
                                             const MVector<const char *> &includes) {
  std::string ret;
  for (const auto &inc : includes) {
    ret += load_shader_source(inc);
    ret += '\n';
  }
  ret += load_shader_source(path);
  return ret;
}

Uint load_shader(const char *path, const MVector<const char *> &includes, Uint shader_type) {
  /* Open the shader file. */
  std::string source = load_shader_source_with_includes(path, includes);
  const char *source_cstr = source.c_str();
  /* Create the shader. */
  Uint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &source_cstr, nullptr);
  glCompileShader(shader);
  /* Check for compalation errors. */
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(shader, 512, nullptr, info_log);
    printf("Error compiling shader: %s\n", info_log);
  }
  return shader;
}

Uint create_shader_program(const MVector<Pair<const char *, Uint>> &parts, const MVector<const char *> &includes) {
  /* Load shaders. */
  MVector<Uint> shaders;
  for (const auto &pair : parts) {
    shaders.push_back(load_shader(pair.first, includes, pair.second));
  }
  /* Create shader program. */
  Uint program = glCreateProgram();
  /* Attach all shaders to the shader program. */
  for (const auto &shader : shaders) {
    glAttachShader(program, shader);
  }
  glLinkProgram(program);
  /* Check for linking errors. */
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetProgramInfoLog(program, 512, nullptr, info_log);
    fprintf(stderr, "Error linking shader program: %s\n", info_log);
  }
  /* Delete shaders used to create shader program. */
  for (const auto &shader : shaders) {
    glDeleteShader(shader);
  }
  return program;
}

Uint create_comp_shader_program(const char *path) {
  return create_shader_program({{path, GL_COMPUTE_SHADER}}, {});
}