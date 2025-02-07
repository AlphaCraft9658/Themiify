#pragma once

#include <vector>

#include "screen.h"
#include "installer.h"

class InstalledThemesScreen : public Screen
{
public:
    InstalledThemesScreen();
    virtual ~InstalledThemesScreen();

    void Draw();
    
    bool Update(VPADStatus status);
private:
    enum MenuState {
        MENU_STATE_INIT,
        MENU_STATE_DIR_ITERATOR,
        MENU_STATE_DEFAULT,
        MENU_STATE_THEME_SHOW_DETAILS
    };

    std::string RegionToString(std::string region);
        
    MenuState mMenuState;

    bool mMenuStateFailure, mNoInstalledThemesFound;

    std::vector<std::string> mFileList, mThemeNames;
    std::vector<Installer::installed_theme_data> mThemeDataList;
        
    int mThemeIdx, mScrollOffset;

    Installer::installed_theme_data mSelectedThemeData;
};