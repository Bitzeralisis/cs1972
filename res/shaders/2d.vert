#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 2) in vec2 texCoord; // UV texture coordinates
uniform mat4 m;
uniform mat4 p;

out vec2 texc;

void main(){
    texc = texCoord;
    gl_Position = p * m * vec4(position, 1.0);
}
