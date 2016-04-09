#version 330 core
out vec4 fragColor;

uniform vec4 color;

uniform sampler2D tex;
uniform int useTexture = 0;

in vec2 texc;

void main() {
    if (useTexture > 0) {
        fragColor = color * texture2D(tex,texc);
    } else {
        fragColor = color;
    }
}
