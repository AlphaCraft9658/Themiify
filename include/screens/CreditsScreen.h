#pragma once

#include "screen.h"

class CreditsScreen : public Screen
{
public: 
    CreditsScreen();
    virtual ~CreditsScreen();

    void Draw();

    bool Update(VPADStatus status);
private:
    void ColourLoop();
};