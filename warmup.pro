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
    src/engine/voxel/voxelmanager.cpp \
    src/util/commonincludes.cpp \
    src/csm/csm_collide.cpp \
    src/util/obj.cpp \
    src/game/platformergame.cpp \
    src/game/gamescreen.cpp \
    src/engine/geometric/geometricmanager.cpp \
    src/game/entity/playerentity.cpp \
    src/game/mainmenuscreen.cpp \
    src/engine/graphics_dr.cpp \
    src/game/entity/playershotentity.cpp \
    src/game/entity/enemyentity.cpp

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
    src/engine/voxel/chunk.h \
    src/engine/voxel/block.h \
    src/engine/voxel/chunkgenerator.h \
    src/engine/voxel/chunkdef.h \
    src/engine/terrainmanager.h \
    src/engine/voxel/voxelmanager.h \
    src/csm/csm.h \
    src/csm/csm_collide.h \
    src/csm/csm_primitive.h \
    src/util/NewCylinderData.h \
    src/util/obj.h \
    src/util/SphereData.h \
    src/game/platformergame.h \
    src/game/gamescreen.h \
    src/engine/geometric/geometricmanager.h \
    src/game/entity/playerentity.h \
    src/game/mainmenuscreen.h \
    src/game/entity/platformerentity.h \
    src/game/entity/playershotentity.h \
    src/game/entity/enemyentity.h


FORMS += src/mainwindow.ui

RESOURCES += \
    res/shaders/shaders.qrc \
    res/images/images.qrc \
    res/obj/obj.qrc

OTHER_FILES += \
    res/obj/level_easy.obj \
    res/obj/level_easy_navmesh.obj \
    res/obj/level_hard.obj \
    res/obj/level_island.obj \
    res/obj/level_island_navmesh.obj \
    res/images/mainmenu.png

DISTFILES += \
    res/shaders/shader.vert \
    res/shaders/shader.frag \
    res/shaders/2d.vert \
    res/shaders/2d.frag \
    res/shaders/gbuffer.vert \
    res/shaders/gbuffer.frag \
    res/shaders/deferred.vert \
    res/shaders/deferred.frag
