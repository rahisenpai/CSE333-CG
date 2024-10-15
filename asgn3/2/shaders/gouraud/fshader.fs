#version 330 core

in vec3 n;
in vec3 e;
in vec3 l;
in vec3 vColor;

out vec4 outColor;

void main(void) {
        outColor = vec4(vColor, 1.0);
}
