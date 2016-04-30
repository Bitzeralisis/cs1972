#version 330 core

layout(location = 0) out vec3 gNormal;
layout(location = 1) out vec3 gColor;
layout(location = 2) out vec3 gGlow;

uniform sampler2D particle_tex;
in vec2 texc;
in float lifef;
in vec3 colorv;

void main() {
    gNormal = vec3(0.f);
    gColor = vec3(0.f);
    gGlow = texture2D(particle_tex, texc).rgb * colorv * min(1.f, lifef);
}
