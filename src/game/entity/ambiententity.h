#pragma once

#include "cogentity.h"

namespace COG {

class AmbientEntity : public COGEntity {
public:
    AmbientEntity(float beat);
    virtual ~AmbientEntity() { }

public:
    virtual void tickBeats(float beats) override;
    virtual void draw(int pass) override;
};

}
