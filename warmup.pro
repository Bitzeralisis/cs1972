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
    src/engine/voxel/chunk.cpp \
    src/game/minecraftgame.cpp \
    src/game/gamescreen.cpp \
    src/engine/voxel/voxelmanager.cpp \
    src/game/randomchunkgenerator.cpp \
    src/game/pnchunkgenerator.cpp \
    src/game/entity/playerentity.cpp \
    src/game/entity/enemyentity.cpp \
    src/game/entity/playerattackentity.cpp \
    src/util/commonincludes.cpp \
    src/game/entity/playershotentity.cpp \
    src/game/entity/stormentity.cpp \
    src/game/mainmenu.cpp

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
    src/engine/entity.h \
    src/engine/voxel/chunk.h \
    src/engine/voxel/block.h \
    src/game/minecraftgame.h \
    src/engine/voxel/chunkgenerator.h \
    src/engine/voxel/chunkdef.h \
    src/game/gamescreen.h \
    src/engine/terrainmanager.h \
    src/engine/voxel/voxelmanager.h \
    src/game/randomchunkgenerator.h \
    src/game/pnchunkgenerator.h \
    src/util/csm.h \
    src/game/entity/minecraftentity.h \
    src/game/entity/playerentity.h \
    src/game/entity/enemyentity.h \
    src/game/entity/playerattackentity.h \
    src/game/entity/playershotentity.h \
    src/game/entity/stormentity.h \
    src/game/mainmenu.h


FORMS += src/mainwindow.ui

RESOURCES += \
    res/shaders/shaders.qrc \
    res/images/images.qrc

OTHER_FILES += \
    res/images/terrain.png \
    res/images/lose.png \
    res/images/win.png \
    res/images/chaostime.png \
    res/images/howto1.png \
    res/images/howto2.png

DISTFILES += \
    res/shaders/shader.vert \
    res/shaders/shader.frag \
    res/shaders/2d.vert \
    res/shaders/2d.frag
