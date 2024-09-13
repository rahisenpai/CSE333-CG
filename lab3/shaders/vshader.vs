#version 330 core
 in vec3 vVertex;
 in vec3 vertex_colour;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;


out vec3 colour;

void main()
{
  
  gl_Position =   vProjection * vView * vModel * vec4(vVertex.x, vVertex.y, vVertex.z, 1.0);
  colour = vertex_colour;

 
}