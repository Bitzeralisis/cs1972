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
    src/game/warmupgame.cpp \
    src/engine/game.cpp \
    src/engine/camera.cpp \
    src/game/mainmenu.cpp \
    src/engine/graphics.cpp \
    src/game/gamescreen.cpp

HEADERS += \
    libs/glew-1.10.0/include/GL/glew.h \
    src/mainwindow.h \
    src/view.h \
    src/util/CommonIncludes.h \
    src/util/ResourceLoader.h \
    src/util/CylinderData.h \
    src/engine/game.h \
    src/game/warmupgame.h \
    src/engine/screen.h \
    src/engine/camera.h \
    src/game/mainmenu.h \
    src/engine/graphics.h \
    src/game/gamescreen.h


FORMS += src/mainwindow.ui

RESOURCES += \
    res/shaders/shaders.qrc \
    res/images/images.qrc

OTHER_FILES += \
    res/images/grass.png \

DISTFILES += \
    res/shaders/shader.vert \
    res/shaders/shader.frag
