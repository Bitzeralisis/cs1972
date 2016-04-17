#include "uishadermodule.h"
#include "../graphics.h"
#include "../primitive.h"
#include "../../util/ResourceLoader.h"

using namespace CS1972Engine;

UiShaderModule::UiShaderModule(Graphics *parent)
    : m_parent(parent)
{
    m_shader = ResourceLoader::loadShaders(":/shaders/2d.vert", ":/shaders/2d.frag");
}

UiShaderModule::~UiShaderModule() {
    glDeleteProgram(m_shader);
}

void UiShaderModule::use() {
    m_parent->useShader(m_shader);
}

void UiShaderModule::orthoTransform(float left, float right, float bottom, float top) {
    m_left = left;
    m_right = right;
    m_bottom = bottom;
    m_top = top;
    glm::mat4 m = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "p"), 1, GL_FALSE, glm::value_ptr(m));
}

void UiShaderModule::color(glm::vec4 color) {
    glUniform4fv(glGetUniformLocation(m_parent->activeShader(), "color"), 1, glm::value_ptr(color));
}

void UiShaderModule::drawQuad(float left, float right, float bottom, float top) {
    drawQuad(left, right, bottom, top, 0.f, 1.f, 0.f, 1.f);
}

void UiShaderModule::drawQuad(float left, float right, float bottom, float top, float texLeft, float texRight, float texBottom, float texTop) {
    drawQuad(left, right, bottom, top, 0.f, texLeft, texRight, texBottom, texTop);
}

void UiShaderModule::drawQuad(float left, float right, float bottom, float top, float rotation, float texLeft, float texRight, float texBottom, float texTop) {
    glm::mat4 m(1.f);
    m = glm::translate(m, glm::vec3(left, bottom, 0.f));
    m = glm::scale(m, glm::vec3(right-left, top-bottom, 1.f));
    m = glm::translate(m, glm::vec3(0.5f, 0.5f, 0.f));
    m = glm::rotate(m, rotation/180.f*glm::pi<float>(), glm::vec3(0.f, 0.f, -1.f));
    m = glm::translate(m, glm::vec3(-0.5f, -0.5f, 0.f));

    glUniformMatrix4fv(glGetUniformLocation(m_parent->activeShader(), "m"), 1, GL_FALSE, glm::value_ptr(m));
    glUniform2f(glGetUniformLocation(m_parent->activeShader(), "tex_bl"), texLeft, texBottom);
    glUniform2f(glGetUniformLocation(m_parent->activeShader(), "tex_ur"), texRight, texTop);
    m_parent->uiQuad()->drawArray();
}

glm::vec3 UiShaderModule::cameraSpaceToUisSpace(glm::vec3 pos) {
    glm::vec3 retval = m_parent->camera()->orthoProject(pos) * 0.5f + glm::vec3(0.5f);
    retval *= glm::vec3(m_right-m_left, m_top-m_bottom, 1.f);
    retval += glm::vec3(m_left, m_bottom, 0.f);
    return retval;
}
