#version 330 core

in vec3 vVertex;

out vec3 color;

uniform vec3 vertexColors;  // Uniform array for vertex colors
uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

void main()
{
    color = vertexColors;  // Assign the uniform color to the output color
    gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
}
