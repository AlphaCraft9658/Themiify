#pragma once

#include <coreinit/time.h>

#include "screen.h"

class CreditsScreen : public Screen
{
public: 
    CreditsScreen();
    virtual ~CreditsScreen();

    void Draw();

    bool Update(VPADStatus status);
private:
    OSTick mInitialTick;
    void ColourLoop();
};