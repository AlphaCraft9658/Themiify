#pragma once

#include <cstdint>

namespace Gfx {
    bool Init();

    void Shutdown();

    void Clear(uint32_t colour);

    void Draw();

    void Print(int x, int y, const char *fmt, ...);

    void SetBackgroundColour(uint32_t colour);

    uint32_t GetBackgroundColour();
}