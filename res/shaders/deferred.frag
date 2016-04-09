#version 330 core

out vec4 fragColor;

in vec3 eye_worldSpace;
in mat4 invPV;

uniform vec2 screenSize;

uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform sampler2D gDepth;

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
    vec2 texc = gl_FragCoord.xy / screenSize;

    vec3 normal_worldSpace = texture2D(gNormal, texc).xyz;
    vec4 base_color = texture2D(gColor, texc);
    float depth_clamp = texture2D(gDepth, texc).x * 2.f - 1.f;

    vec2 worldc = texc * 2.f - vec2(1.f);
    vec4 posn = invPV * vec4(worldc.xy, depth_clamp, 1.f);
    vec3 position_worldSpace = posn.xyz / posn.w;

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
        fragColor = vec4(lightColor * base_color.xyz * brightness, 1.f);
    } else {
        fragColor = vec4(lightColor * base_color.xyz, 1.f);
    }
}
