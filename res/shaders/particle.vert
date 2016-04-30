#version 330 core

layout(location = 0) in vec2 texc;

uniform mat4 v;

uniform sampler2D position_tex;
uniform sampler2D color_tex;

out float life;
out vec3 color;

void main() {
    vec4 poslife = texture2D(position_tex, texc).rgba;
    gl_Position = v * vec4(poslife.xyz, 1.f);
    life = poslife.w;
    color = texture2D(color_tex, texc).rgb;
}
