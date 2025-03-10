#pragma once

#include <string>
#include <filesystem>

#include <whb/log.h>

#define THEMIIFY_ROOT "fs:/vol/external01/themiify"
#define THEMIIFY_INSTALLED_THEMES "fs:/vol/external01/themiify/installed"
#define THEMES_ROOT "fs:/vol/external01/wiiu/themes"

#define WII_U_MENU_JPN_TID (0x0005001010040000)
#define WII_U_MENU_USA_TID (0x0005001010040100)
#define WII_U_MENU_EUR_TID (0x0005001010040200)

#define BACKGROUND_COLOUR (0x00382AFF) // waiting for Perro to decide on this one lmao
#define BACKGROUND_ERR_COLOUR (0x750000FF)
#define BACKGROUND_WARNING_COLOUR (0xAAAA00FF)
#define BACKGROUND_SUCCESS_COLOUR (0x007500FF)

inline bool CreateParentDirectories(std::string inputPath) {
    std::filesystem::path fullPath = inputPath;
    std::filesystem::path parentPath = fullPath.parent_path();
        
    if (!(std::filesystem::create_directories(parentPath))) {
        return false;
    }

    return true;
}