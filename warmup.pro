QT += core gui opengl

TARGET = warmup
TEMPLATE = app

win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lopengl32
}
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += src libs glm libs/glew-1.10.0/include
DEPENDPATH += src libs glm libs/glew-1.10.0/include

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/view.cpp \
    src/util/ResourceLoader.cpp \
    libs/glew-1.10.0/src/glew.c \
    src/engine/game.cpp \
    src/engine/camera.cpp \
    src/engine/graphics.cpp \
    src/engine/primitive.cpp \
    src/engine/world.cpp \
    src/engine/entity.cpp

HEADERS += \
    libs/glew-1.10.0/include/GL/glew.h \
    src/mainwindow.h \
    src/view.h \
    src/util/CommonIncludes.h \
    src/util/ResourceLoader.h \
    src/util/CylinderData.h \
    src/engine/game.h \
    src/engine/screen.h \
    src/engine/camera.h \
    src/engine/graphics.h \
    src/util/HeaderUtils.h \
    src/engine/primitive.h \
    src/engine/world.h \
    src/engine/entity.h


FORMS += src/mainwindow.ui

RESOURCES += \
    res/shaders/shaders.qrc \
    res/images/images.qrc

DISTFILES += \
    res/shaders/shader.vert \
    res/shaders/shader.frag \
    res/shaders/2d.vert \
    res/shaders/2d.frag
