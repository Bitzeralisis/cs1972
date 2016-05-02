#pragma once

#include <deque>
#include <unordered_map>
#include <glm/common.hpp>

namespace COG {

// Forward declarations
struct COGScript;
struct COGScriptBehavior;

// Enum types
enum class Attribute {
    UNDEFINED,
    // Spatial attributes
    POSITION,
    VELOCITY,
    ACCELERATION,
    // Single-value attributes
    TARGETABLE,
};

enum class Action {
    UNDEFINED,
    // Simple actions (no arguments)
    DISAPPEAR,
    // Attributes
    SET_ATTRIBUTE,
    // Advanced
    SPAWN,
    SHOOT,
    PLAY_MUSIC,
};

// Action types
struct COGScriptAction {
    bool queued;
    float beat;
    Action action;
    COGScriptAction() = delete;
    COGScriptAction(Action type) : action(type) { }
};

// Action types - attributes
struct COGScriptActionSetAttribute : public COGScriptAction {
    Attribute attr;
    COGScriptActionSetAttribute() : COGScriptAction(Action::SET_ATTRIBUTE) { }
};

struct COGScriptAttributeSpatial : public COGScriptActionSetAttribute {
    bool relative;
    glm::vec3 coord;
};

struct COGScriptAttributeFloat : public COGScriptActionSetAttribute {
    float value;
};

// Action types - advanced
struct COGScriptActionSpawn : public COGScriptAction {
    std::string type;
    std::string behavior;
    COGScriptBehavior *init;
    COGScriptActionSpawn() : COGScriptAction(Action::SPAWN) { }
    ~COGScriptActionSpawn();
};

struct COGScriptActionShoot : public COGScriptAction {
    float travel;
    COGScriptAttributeSpatial pos;
    COGScriptAttributeSpatial vel;
    int type;
    int lane;
    COGScriptActionShoot() : COGScriptAction(Action::SHOOT) { }
};

struct COGScriptActionPlay : public COGScriptAction {
    std::string sound;
    float bpm;
    float offset;
    float loopStart;
    float loopEnd;
    COGScriptActionPlay() : COGScriptAction(Action::PLAY_MUSIC) { }
};

// Script constructs
struct COGScriptBehavior {
    COGScript *const parent;
    std::deque<COGScriptAction *> actions;
    COGScriptBehavior() = delete;
    COGScriptBehavior(COGScript *script) : parent(script) { }
    ~COGScriptBehavior();
};

struct COGScript {
    std::unordered_map<std::string, COGScriptBehavior *> behaviors;
    ~COGScript();
};

}
