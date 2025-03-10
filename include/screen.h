#pragma once

#include <vpad/input.h>

class Screen
{
public:
    Screen() = default;
    virtual ~Screen() = default;

    virtual void Draw() = 0;

    virtual bool Update(VPADStatus status) = 0;
};