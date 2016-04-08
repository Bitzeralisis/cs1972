#include "shadermodule.h"
#include "../graphics.h"
#include "../../util/ResourceLoader.h"

using namespace CS1972Engine;

ShaderModule::ShaderModule(Graphics *parent)
    : m_parent(parent)
{
    m_shader = ResourceLoader::loadShaders(":/shaders/shader.vert", ":/shaders/shader.frag");
}

ShaderModule::~ShaderModule() {
    glDeleteProgram(m_shader);
}

void ShaderModule::use() {
    m_parent->useShader(m_shader);
}

void ShaderModule::pvTransformFromCamera() {
    glm::mat4 p = m_parent->camera()->perspectiveMatrix();
    glm::mat4 v = m_parent->camera()->viewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "p"), 1, GL_FALSE, glm::value_ptr(p));
    glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "v"), 1, GL_FALSE, glm::value_ptr(v));
}

void ShaderModule::mTransform(glm::mat4 m) {
    glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "m"), 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderModule::color(glm::vec4 color) {
    glUniform4fv(glGetUniformLocation(m_parent->activeShader(), "color"), 1, glm::value_ptr(color));
}

void ShaderModule::useTexture(bool use) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useTexture"), use);
}

void ShaderModule::bindTexture(GLuint tex) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "tex"), GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
}

void ShaderModule::bindTexture(const char *name) {
    bindTexture(m_parent->getTexture(name));
}

void ShaderModule::unbindTexture() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ShaderModule::useFog(bool use) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useFog"), use);
}

void ShaderModule::useFog(bool use, float fogNear, float fogFar, glm::vec3 color) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useFog"), use);
    glUniform1f(glGetUniformLocation(m_parent->activeShader(), "fogNear"), fogNear);
    glUniform1f(glGetUniformLocation(m_parent->activeShader(), "fogFar"), fogFar);
    glUniform3fv(glGetUniformLocation(m_parent->activeShader(), "fogColor"), 1, glm::value_ptr(color));
}

void ShaderModule::useLight(bool use) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useLight"), use);
}

void ShaderModule::useLight(bool use, int type, glm::vec3 pos) {
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "useLight"), use);
    glUniform1i(glGetUniformLocation(m_parent->activeShader(), "lightType"), type);
    glUniform3fv(glGetUniformLocation(m_parent->activeShader(), "lightPosition"), 1, glm::value_ptr(pos));
}
