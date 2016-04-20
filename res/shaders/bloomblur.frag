#version 330 core
out vec3 fragColor;

uniform vec2 direction;
uniform vec2 screenSize;
// 1d kernel for a 33 x 33 blur
uniform float weight[17] = float[] (0.047106, 0.046823, 0.045983, 0.044617, 0.042772, 0.040512, 0.037911, 0.035052, 0.03202, 0.028899, 0.02577, 0.022704, 0.019763, 0.016997, 0.014442, 0.012125, 0.010057);
uniform sampler2D tex;

in vec2 texc;

void main() {
    vec2 dir = direction/screenSize;
    vec3 color = weight[0] * texture2D(tex, texc).rgb;
    for (int i = 1; i < 17; i += 2) {
        float total = weight[i]+weight[i+1];
        float pos = weight[i+1] / total + i;
        color += total * texture2D(tex, texc + pos*dir).rgb;
        color += total * texture2D(tex, texc - pos*dir).rgb;
    }
    fragColor = color;
}
