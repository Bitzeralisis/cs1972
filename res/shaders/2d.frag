#version 330 core
out vec4 fragColor;

uniform vec4 color;

uniform sampler2D tex;
uniform int useTexture = 0;

in vec2 texc;

void main() {
    if (useTexture > 0) {
        fragColor = color * clamp(texture2D(tex,texc), 0.f, 1.f);
    } else {
        fragColor = color;
    }
}
