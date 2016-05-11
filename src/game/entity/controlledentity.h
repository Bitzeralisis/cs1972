#pragma once

#include "cogentity.h"

namespace COG {

struct COGScriptAction;
struct COGScriptBehavior;

class ControlledEntity : public COGEntity {
public:
    ControlledEntity(float beat);
    virtual ~ControlledEntity();

private:
    int m_step = 0;
    const COGScriptBehavior *m_behavior = 0;
    float m_scriptOffset;

public:
    VALUE_ACCESSOR(const COGScriptBehavior *,behavior)
    void giveBehavior(const COGScriptBehavior *behavior) { m_step = 0; m_behavior = behavior; }
    virtual void performAction(COGScriptAction *action);

    void shoot(float beat, float travelTime, glm::vec3 pos, glm::vec3 vel, int type, int lane);

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override { }
};

}
