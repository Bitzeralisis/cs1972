#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 2) in vec2 texCoord; // UV texture coordinates

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

out vec2 texc;
out vec3 eye_worldSpace;

void main(){
    gl_Position = p * m * vec4(position, 1.0);
    texc = (gl_Position.xy / gl_Position.w) * 0.5f + 0.5f;
    eye_worldSpace = (inverse(v)*vec4(0,0,0,1)).xyz;
}
