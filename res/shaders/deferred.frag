#version 330 core

out vec4 fragColor;

in vec2 texc;
in vec3 eye_worldSpace;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;

/*uniform int useFog = 0;
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;*/

uniform int useLight = 1;
uniform int lightType = 1;
uniform vec3 lightPosition = vec3(10.f, 10.f, 5.f);
uniform vec3 lightColor = vec3(1.f, 1.f, 1.f);
uniform vec3 lightAtten = vec3(1.f, 0.f, 0.f);

void main() {
    vec4 base_color = texture2D(gColor, texc);
    vec3 position_worldSpace = texture2D(gPosition, texc).xyz;
    vec3 normal_worldSpace = texture2D(gNormal, texc).xyz;

    if (useLight > 0) {
        vec3 vertexToLight = vec3(0.f);
        float brightness = 1.f;
        if (lightType == 1) {
            // Directional light
            vertexToLight = normalize(lightPosition);
            brightness = 1.f;
        } else if (lightType == 2) {
            // Point light
            vertexToLight = normalize(lightPosition - position_worldSpace);
            float dist = distance(lightPosition, position_worldSpace);
            brightness = min(1.f / dot(lightAtten, vec3(1.f, dist, dist*dist)), 1.f);
        }

        // Diffuse lighting
        float diffuseIntensity = max(0.f, dot(vertexToLight, normal_worldSpace));

        // Specular lighting
        vec3 lightReflection = normalize(-reflect(vertexToLight, normal_worldSpace));
        vec3 eyeDirection = normalize(eye_worldSpace - position_worldSpace);
        float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), 100);

        brightness *= mix(diffuseIntensity, specIntensity, 0.2f);
        fragColor = vec4(lightColor, 1.f) * base_color * brightness;
    } else {
        fragColor = vec4(lightColor, 1.f) * base_color;
    }

    fragColor.a = 1.f;

    // Fog
    /*if (useFog > 0) {
        float fog = min(max(0.f, (length(position_worldSpace-eye_worldSpace)-fogNear)/(fogFar-fogNear)), 1.f);
        fragColor = vec4(fog*fogColor + (1.f-fog)*fragColor.xyz, fragColor.w);
    }*/
}
