#version 330 core

in vec3 n;
in vec3 e;
in vec3 l;
out vec4 outColor;

vec3 Ls = vec3(1.0, 1.0, 1.0);
vec3 Ld = vec3(0.7, 0.7, 0.7);
vec3 La = vec3(0.6, 0.3, 0.4);

vec3 ks = vec3(1.0, 1.0, 1.0);
vec3 kd = vec3(0.5, 0.6, 0.4);
vec3 ka = vec3(1.0, 1.0, 1.0);

float spec_exp = 32;

//spotlight properties
uniform vec3 spotlightDir;
uniform float cutOff;
uniform float inCutOff;

void main(void) {
        vec3 Ia = ka * La;//ambient
        vec3 Id = kd * max(0, dot(l,n)) * Ld;//diffuse
        vec3 Is = ks * Ls * max(0, pow(dot(n, normalize(l + e)), spec_exp));//specular

        float theta = dot(normalize(-l), normalize(spotlightDir)); //angle between fragment-to-light and spotlight direction
        if (theta > inCutOff) { //inside the spotlight cone: apply lighting
                vec3 fColor = Ia + Id + Is;
                outColor = vec4(fColor, 1.0);
        }
        else if (theta <= inCutOff && theta > cutOff) { //linearly interpolate the intensity
                float intensity = (theta - cutOff) / (inCutOff - cutOff);
                vec3 fColor = (Id + Is) * intensity + Ia;
                outColor = vec4(fColor, 1.0);
        }
        else { //outside the spotlight cone: only ambient light
                outColor = vec4(Ia, 1.0);
        }
}
