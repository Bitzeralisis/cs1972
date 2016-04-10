#version 330 core
out vec4 fragColor;

uniform sampler2D poslife_tex;
uniform sampler2D vel_tex;
uniform float timestep;

in vec2 texc;

void main() {
    vec4 poslife = texture2D(poslife_tex, texc).rgba;
    vec3 vel = texture2D(vel_tex, texc).rgb;
    fragColor = vec4(poslife.rgb + timestep*vel.rgb, max(0.f, poslife.a - timestep));
}
