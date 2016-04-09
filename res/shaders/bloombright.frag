#version 330 core
out vec3 fragColor;

uniform sampler2D tex;

in vec2 texc;

void main() {
    fragColor = max(texture2D(tex,texc).rgb-vec3(1.f), 0.f);
}
