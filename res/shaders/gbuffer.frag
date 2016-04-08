#version 330 core

layout(location = 0) out vec3 gNormal;
layout(location = 1) out vec3 gColor;
layout(location = 2) out vec3 gGlow;

in vec4 position_worldSpace;
in vec4 normal_worldSpace;
in vec3 eye_worldSpace;
in vec2 texc;

uniform vec4 color;
uniform int useTexture = 0;
uniform sampler2D tex;

uniform vec4 glow;
uniform int useGlowTexture = 0;
uniform sampler2D glowTex;

uniform int useFog = 0;
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

void main() {
    gNormal = normalize(normal_worldSpace).xyz;
    gColor = color.rgb;
    gGlow = glow.rgb;
    if (useTexture > 0) {
        gColor = color.rgb * texture2D(tex, texc).rgb;
    }
    if (useGlowTexture > 0) {
        gGlow = glow.rgb * texture2D(glowTex, texc).rgb;
    }

    // Fog
    if (useFog > 0) {
        float fog = min(max(0.f, (distance(position_worldSpace.xyz, eye_worldSpace)-fogNear)/(fogFar-fogNear)), 1.f);
        gColor = mix(gColor, vec3(0.f), fog);
        gGlow = mix(gGlow, fogColor, fog);
    }
}
