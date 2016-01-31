#include "mainmenu.h"
#include "../util/CommonIncludes.h"

using namespace WarmupGame;

MainMenu::~MainMenu() {

}

void MainMenu::tick() {

}

void MainMenu::draw() {
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);
    glEnd();
}

void MainMenu::mousePressEvent(QMouseEvent *event) {

}

void MainMenu::mouseMoveEvent(QMouseEvent *event) {

}

void MainMenu::mouseReleaseEvent(QMouseEvent *event) {

}

void MainMenu::wheelEvent(QWheelEvent *event) {

}

void MainMenu::keyPressEvent(QKeyEvent *event) {

}

void MainMenu::keyReleaseEvent(QKeyEvent *event) {

}

void MainMenu::resizeEvent(int w, int h) {

}

