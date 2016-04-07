#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gColor;

in vec4 position_worldSpace;
in vec4 normal_worldSpace;
in vec2 texc;

uniform vec4 color;
uniform int useTexture = 0;
uniform sampler2D tex;

void main() {
    gPosition = position_worldSpace.xyz;
    gNormal = normalize(normal_worldSpace).xyz;
    gColor = color;
    if (useTexture > 0) {
        gColor = color * clamp(texture2D(tex, texc), 0.f, 1.f);
    }
}
