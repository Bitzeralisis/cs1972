#version 330 core
out vec4 fragColor;

uniform vec4 color;

uniform int useTexture = 0;
uniform sampler2D tex;

uniform int useFog = 0;
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

uniform int useLight = 1;

in vec4 normal_worldSpace;
in vec4 position_worldSpace;
in vec4 eye_worldSpace;
in vec2 texc;

void main( ){
    fragColor = vec4(0.f);

    vec4 base_color = vec4(0.f);
    if (useTexture > 0) {
        base_color = color * clamp(texture2D(tex, texc), 0.f, 1.f);
    } else {
        base_color = color;
    }

    if (useLight > 0) {
        fragColor += vec4(0.5f*base_color.xyz, base_color.w);

        // Diffuse lighting
        vec4 vertexToLight = normalize(vec4(1.f, 1.f, 0.5f, 0.f));
        float diffuseIntensity = max(0.f, dot(vertexToLight, normal_worldSpace));
        fragColor += 0.5f*vec4(max(vec3(0.f), vec3(1.f) * base_color.xyz * diffuseIntensity), 0.f);

        // Specular lighting
        vec4 lightReflection = normalize(-reflect(vertexToLight, normal_worldSpace));
        vec4 eyeDirection = normalize(eye_worldSpace - position_worldSpace);
        float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), 100);
        fragColor += 0.1f*vec4(max(vec3(0.f), vec3(1.f) * base_color.xyz * specIntensity), 0.f);
    } else {
        fragColor = base_color;
    }

    // Fog
    if (useFog > 0) {
        float fog = min(max(0.f, (length(position_worldSpace-eye_worldSpace)-fogNear)/(fogFar-fogNear)), 1.f);
        fragColor = vec4(fog*fogColor + (1.f-fog)*fragColor.xyz, fragColor.w);
    }
}
