#version 450 core

/* Input: triangles from the vertex shader */
layout(triangles) in;
/* Output: one triangle per input triangle */
layout(triangle_strip, max_vertices = 3) out;

/* Input normals from the vertex shader */
in vec3 Normal[];
/* Input fragment positions from the vertex shader */
in vec3 FragPos[];

/* Output normal for the fragment shader */
out vec3 GsNormal;
/* Output fragment position for the fragment shader */
out vec3 GsFragPos;

/* Amount to expand the triangles */
uniform float expansion_factor = 0.0;

void main() {
  /* For each vertex in the triangle, move it along its normal */
  for (int i = 0; i < 3; ++i) {
    vec3 expanded_position = FragPos[i] + Normal[i] * expansion_factor;
    gl_Position = gl_in[i].gl_Position + vec4(Normal[i] * expansion_factor, 0.0);
    /* Pass the normal to the fragment shader */
    GsNormal = Normal[i];
    /* Pass the position to the fragment shader */
    GsFragPos = expanded_position;
    EmitVertex();
  }
  /* Finish the current primitive (triangle) */
  EndPrimitive();
}