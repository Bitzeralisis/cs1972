#pragma once

#include "cogentity.h"

namespace COG {

class PlayerEntity : public COGEntity {
public:
    PlayerEntity()
        : COGEntity(0.f)
    { }
    virtual ~PlayerEntity() { }

public:
    virtual void tickBeats(float beats) override { }
    virtual void draw(int pass) override { }
};

}
