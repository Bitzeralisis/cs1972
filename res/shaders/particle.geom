#version 330 core

const vec2 corners[4] =  vec2[] (vec2(0.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(1.0, 0.0));
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in float life[];
in vec3 color[];

uniform mat4 p;
uniform float particleSize;

out vec2 texc;
out float lifef;
out vec3 colorv;

void main() {
    if (life[0] > 0.f) {
        for (int i = 0; i < 4; ++i) {
            vec4 eyePos = gl_in[0].gl_Position;
            eyePos.xy += particleSize * (corners[i] - vec2(0.5));
            gl_Position = p * eyePos;
            texc = corners[i];
            lifef = life[0];
            colorv = color[0];
            EmitVertex();
        }
    }
}
