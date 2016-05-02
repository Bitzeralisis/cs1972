QT += core gui opengl

TARGET = warmup
TEMPLATE = app

win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lopengl32 -L$$PWD/libs/fmod-win32/lib -lfmod
    INCLUDEPATH += libs/fmod-win32/inc
    DEPENDPATH += libs/fmod-win32/inc
}
unix {
    LIBS += -L$$PWD/libs/fmod-unix/lib/x86_64 -lfmod
    INCLUDEPATH += libs/fmod-unix/inc
    DEPENDPATH += libs/fmod-unix/inc
}
QMAKE_CXXFLAGS += -lpthread -std=c++11
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

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
    src/engine/graphics/shadermodule.cpp \
    src/engine/graphics/deferredmodule.cpp \
    src/csm/csm.cpp \
    src/engine/graphics/uishadermodule.cpp \
    src/engine/graphics/bloommodule.cpp \
    src/engine/graphics/particlemodule.cpp \
    src/engine/audio.cpp \
    src/engine/sound.cpp \
    src/game/entity/lightentity.cpp \
    src/game/gamescreen.cpp \
    src/game/entity/enemyentity.cpp \
    src/game/entity/playershotentity.cpp \
    src/engine/enginedef.cpp \
    src/game/coggame.cpp \
    src/game/entity/playerentity.cpp \
    src/game/entity/ambiententity.cpp \
    src/game/entity/enemy/diamondaenemy.cpp \
    src/game/entity/enemy/cubeaenemy.cpp \
    src/game/entity/enemyshotentity.cpp \
    src/game/scriptparser.cpp \
    src/game/entity/controlledentity.cpp \
    src/game/cogscript.cpp \
    src/game/entity/cogentity.cpp

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
    src/engine/graphics/uishadermodule.h \
    src/engine/graphics/bloommodule.h \
    src/engine/graphics/particlemodule.h \
    src/engine/audio.h \
    src/engine/sound.h \
    src/game/entity/lightentity.h \
    src/game/entity/cogentity.h \
    src/game/gamescreen.h \
    src/game/entity/enemyentity.h \
    src/game/entity/playerentity.h \
    src/game/entity/playershotentity.h \
    src/engine/enginedef.h \
    src/game/enginedef.h \
    src/game/entity/ambiententity.h \
    src/game/entity/enemy/diamondaenemy.h \
    src/game/entity/enemy/cubeaenemy.h \
    src/game/entity/enemyshotentity.h \
    src/game/scriptparser.h \
    src/game/entity/controlledentity.h \
    src/game/cogscript.h


FORMS += src/mainwindow.ui

RESOURCES += \
    res/shaders/shaders.qrc \
    res/images/images.qrc \
    res/obj/obj.qrc \
    res/level/level.qrc

OTHER_FILES += \
    res/images/cube1.png \
    res/images/cube2.png \
    res/images/diamond1.png \
    res/images/particle1.png \
    res/images/particle2.png \
    res/images/particle3.png \
    res/images/reticle.png \
    res/images/enemyshot.png \
    res/images/target.png \
    res/images/hud.png \
    res/level/level.txt

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
    res/shaders/bloomblur.frag \
    res/shaders/particlephysics.frag \
    res/shaders/particle.vert \
    res/shaders/particle.geom \
    res/shaders/particle.frag
