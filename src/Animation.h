#pragma once

#include "Ring.h"

class Animation {
public:
    explicit Animation(Ring &aRing) : ring(aRing) {};

    virtual void execute() = 0;

    virtual ~Animation() = default;

protected:
    Ring &ring;
};
