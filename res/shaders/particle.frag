#version 330 core

layout(location = 0) out vec3 gNormal;
layout(location = 1) out vec3 gColor;
layout(location = 2) out vec3 gGlow;

in float dist;

void main() {
    gNormal = vec3(0.f);
    gColor = vec3(0.f);
    gGlow = vec3(max(0.f, 5.f-dist/10.f));
}
