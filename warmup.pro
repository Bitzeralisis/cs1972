QT += core gui opengl

TARGET = warmup
TEMPLATE = app

win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lopengl32
}
QMAKE_CXXFLAGS += -lpthread -std=c++11

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
    src/engine/entity.cpp \
    src/util/commonincludes.cpp \
    src/csm/csm_collide.cpp \
    src/util/obj.cpp \
    src/engine/graphics_bloom.cpp \
    src/engine/graphics/shadermodule.cpp \
    src/engine/graphics/deferredmodule.cpp \
    src/game/initscreen.cpp \
    src/csm/csm.cpp \
    src/engine/graphics/uishadermodule.cpp \
    src/engine/graphics/bloommodule.cpp

HEADERS += \
    libs/glew-1.10.0/include/GL/glew.h \
    src/mainwindow.h \
    src/view.h \
    src/util/CommonIncludes.h \
    src/util/ResourceLoader.h \
    src/engine/game.h \
    src/engine/screen.h \
    src/engine/camera.h \
    src/engine/graphics.h \
    src/util/HeaderUtils.h \
    src/engine/primitive.h \
    src/engine/world.h \
    src/engine/entity.h \
    src/engine/terrainmanager.h \
    src/csm/csm.h \
    src/csm/csm_collide.h \
    src/csm/csm_primitive.h \
    src/util/NewCylinderData.h \
    src/util/obj.h \
    src/util/SphereData.h \
    src/game/coggame.h \
    src/engine/graphics/shadermodule.h \
    src/engine/graphics/deferredmodule.h \
    src/game/initscreen.h \
    src/engine/graphics/uishadermodule.h \
    src/engine/graphics/bloommodule.h


FORMS += src/mainwindow.ui

RESOURCES += \
    res/shaders/shaders.qrc \
    res/images/images.qrc \
    res/obj/obj.qrc

OTHER_FILES += \


DISTFILES += \
    res/shaders/shader.vert \
    res/shaders/shader.frag \
    res/shaders/2d.vert \
    res/shaders/2d.frag \
    res/shaders/gbuffer.vert \
    res/shaders/gbuffer.frag \
    res/shaders/deferred.vert \
    res/shaders/deferred.frag \
    res/shaders/passthrough.vert \
    res/shaders/bloombright.frag \
    res/shaders/bloomblur.frag
