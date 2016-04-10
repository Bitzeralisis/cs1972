#version 330 core

layout(location = 0) in vec2 texc;

uniform mat4 v;
uniform mat4 p;

uniform sampler2D position_tex;

out float dist;

void main() {
    vec3 pos = texture2D(position_tex, texc).rgb;
    gl_Position = p * v * vec4(pos, 1.f);
    dist = length(pos);
    //gl_PointSize = 5.f;
}
