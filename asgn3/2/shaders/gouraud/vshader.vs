#version 330 core

in vec3 vVertex;
in vec3 vNormal;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
uniform vec3 lpos_world;
uniform vec3 eye_normal;

out vec3 n;
out vec3 e;
out vec3 l;
out vec3 vColor;

void main() {
	gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
	n = normalize(vNormal);
    l = normalize(lpos_world - vVertex);
    e = normalize(eye_normal);

    vec3 Ls = vec3(1.0, 1.0, 1.0);
    vec3 Ld = vec3(0.7, 0.7, 0.7);
    vec3 La = vec3(0.6, 0.3, 0.4);

    vec3 ks = vec3(1.0, 1.0, 1.0);
    vec3 kd = vec3(0.5, 0.6, 0.4);
    vec3 ka = vec3(1.0, 1.0, 1.0);

    float spec_exp = 32;

    vec3 Ia = ka * La;//ambient

    vec3 Id = kd * max(0, dot(l,n)) * Ld;//diffuse

    vec3 Is = ks * Ls * max(0.0, pow(dot(reflect(l, n), e), spec_exp));//specular

    vColor = Ia + Id + Is;
}