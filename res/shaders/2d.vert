#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 2) in vec2 texCoord; // UV texture coordinates
uniform mat4 m;
uniform mat4 p;
uniform vec2 tex_bl = vec2(0.f, 0.f);
uniform vec2 tex_ur = vec2(1.f, 1.f);

out vec2 texc;

void main(){
    texc = texCoord * (tex_ur-tex_bl) + tex_bl;
    gl_Position = p * m * vec4(position, 1.0);
}
