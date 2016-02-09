#version 330 core
out vec4 fragColor;

uniform vec3 color;

uniform sampler2D tex;
uniform int useTexture = 0;

in vec2 texc;

void main(){
    vec3 base_color = vec3(0);
    fragColor = vec4(0);
    if(useTexture > 0){
        base_color = clamp(texture2D(tex,texc).xyz,0,1);
    }else{
        base_color = color;
    }
    fragColor = vec4(base_color, 0.f);
}
