#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 2) in vec2 texCoord; // UV texture coordinates

uniform mat4 pvm;
uniform mat4 v;
uniform mat4 p;

out vec3 eye_worldSpace;
out mat4 invPV;

void main() {
    gl_Position = pvm * vec4(position, 1.0);
    eye_worldSpace = (inverse(v)*vec4(0,0,0,1)).xyz;
    invPV = inverse(v) * inverse(p);
}
