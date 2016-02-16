#version 330 core
out vec4 fragColor;

uniform vec3 color;

uniform int useFog = 0;
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

uniform sampler2D tex;
uniform int useTexture = 0;

in vec4 normal_worldSpace;
in vec4 position_worldSpace;
in vec4 eye_worldSpace;
in vec2 texc;

void main(){
    vec3 base_color = vec3(0);
    fragColor = vec4(0);
    if(useTexture > 0){
        base_color = color*clamp(texture2D(tex,texc).xyz,0,1);
    }else{
        base_color = color;
    }
    vec4 vertexToLight = normalize(vec4(1,1,0.5f,0));
    float diffuseIntensity = max(0.0, dot(vertexToLight, normal_worldSpace));
    fragColor += 0.5f*vec4(max(vec3(0), vec3(1,1,1) * base_color * diffuseIntensity),0.f);

    // Add specular component
    vec4 lightReflection = normalize(-reflect(vertexToLight, normal_worldSpace));
    vec4 eyeDirection = normalize(eye_worldSpace - position_worldSpace);
    float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), 100);
    fragColor += 0.1f*vec4(max(vec3(0), vec3(1,1,1) * base_color * specIntensity),0.f);

    fragColor += 0.5f*vec4(base_color,0.f);

    // Fog
    if (useFog > 0) {
        float fog = min(max(0.f, (length(position_worldSpace-eye_worldSpace)-fogNear)/(fogFar-fogNear)), 1.f);
        fragColor = fog*fogColor + (1.f-fog)*fragColor;
    }
}
