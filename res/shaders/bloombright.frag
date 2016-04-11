#version 330 core
out vec3 fragColor;

uniform sampler2D tex;

in vec2 texc;

void main() {
    fragColor = max(texture2D(tex,texc).rgb-vec3(0.5f), 0.f);
}
