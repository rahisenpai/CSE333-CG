#version 330 core

in vec3 n;
in vec3 e;
in vec3 l;
in vec3 vColor;
out vec4 outColor;

vec3 Ls = vec3(1.0, 1.0, 1.0);
vec3 Ld = vec3(0.7, 0.7, 0.7);
vec3 La = vec3(0.6, 0.3, 0.4);

vec3 ks = vec3(1.0, 1.0, 1.0);
vec3 kd = vec3(0.5, 0.6, 0.4);
vec3 ka = vec3(1.0, 1.0, 1.0);

float spec_exp = 5;

//ambient
//vec3 Ia = ?;

//diffuse
//vec3 Id = ?;

//specular
//vec3 Is = ?;

// vec3 fColor = Ia + Id +Is;
vec3 fColor = vec3(0.0, 0.0, 0.0);

void main(void) {
        outColor = vec4(vColor, 1.0);
}
