#pragma once

#include "controlledentity.h"

namespace COG {

class AmbientEntity : public ControlledEntity {
public:
    AmbientEntity(float beat);
    virtual ~AmbientEntity() { }

private:
    int m_zone = 0;
    bool m_doFade = false;
    float m_fade = 0.f;

public:
    VALACC_MUT(int,zone)
    VALACC_MUT(bool,doFade)

    virtual void tickBeats(float beats) override;
    virtual void draw(int pass, float beat) override;
};

}
