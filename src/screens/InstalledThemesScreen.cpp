#include <filesystem>
#include <vector>

#include <whb/log.h>

#include "gfx.h"
#include "installer.h"
#include "utils.h"
#include "InstalledThemesScreen.h"

InstalledThemesScreen::InstalledThemesScreen()
    :   mMenuState(MENU_STATE_INIT),
        mMenuStateFailure(false),
        mThemeIdx(0),
        mScrollOffset(0)
{
}

InstalledThemesScreen::~InstalledThemesScreen()
{
}

void InstalledThemesScreen::Draw()
{
    switch (mMenuState) {
        case MENU_STATE_INIT:
            std::filesystem::create_directories(THEMIIFY_INSTALLED_THEMES);
            break;
        case MENU_STATE_DIR_ITERATOR:
            if (mNoInstalledThemesFound) {
                Gfx::SetBackgroundColour(BACKGROUND_ERR_COLOUR);
                Gfx::Print(-4, 2, "Could not find any themes installed at:\n%s", THEMIIFY_INSTALLED_THEMES);
                Gfx::Print(-4, 17, "                             B - Back");
            }
            else if (mMenuStateFailure) {
                Gfx::SetBackgroundColour(BACKGROUND_ERR_COLOUR);
                Gfx::Print(-4, 2, "Error retrieving the metadata for your\ninstalled themes.");
                Gfx::Print(-4, 17, "                             B - Back");
            }
            else {
                Gfx::Print(-4, 2, "Iterating through\n%s...", THEMIIFY_INSTALLED_THEMES);
            }

            break;
        case MENU_STATE_DEFAULT: {
            Gfx::Print(-3, 2, "Select a theme to view information about it");

            int yIni = 4;
            for (std::size_t i = 0; i < 12 && (mScrollOffset + i) < mFileList.size(); i++) {
                int y = yIni + i;
                std::size_t themeIdx = mScrollOffset + i;
                Gfx::Print(-4, y, (mThemeIdx == static_cast<int>(themeIdx)) ? "> %s" : "  %s", mThemeNames.at(themeIdx).c_str());
            }

            Gfx::Print(-4, 17, "A - Select                D-Pad/Stick - Move               B - Back");
            break;            
        }
        case MENU_STATE_THEME_SHOW_DETAILS:
            // TODO: Figure out the "select default theme" appraoch we're gonna do
            // TODO: Should also maybe add a feature to uninstall mods (Would delete the modpack path exposed by the json and then the json itself)
            Gfx::Print(-4, 2, "Theme Details:");
            Gfx::Print(-3, 4, "Theme Name: %s\nTheme Author: %s\nTheme Region: %s\nTheme ID: %s\n\nTheme was installed to:\n%s", mSelectedThemeData.themeName.c_str(), mSelectedThemeData.themeAuthor.c_str(), RegionToString(mSelectedThemeData.themeRegion).c_str(), mSelectedThemeData.themeID.c_str(), mSelectedThemeData.installedThemePath.c_str());
            Gfx::Print(-4, 17, "                             B - Back");

            break;
    }
}

bool InstalledThemesScreen::Update(VPADStatus status)
{   
    Installer::installed_theme_data themeData;
    
    switch (mMenuState) {
        case MENU_STATE_INIT:
            mNoInstalledThemesFound = false;
            mMenuState = MENU_STATE_DIR_ITERATOR;
            break;
        case MENU_STATE_DIR_ITERATOR:

            mFileList.clear();

            for (const auto & entry : std::filesystem::directory_iterator(THEMIIFY_INSTALLED_THEMES)) {
                if (entry.path().extension() == ".json") {
                    mFileList.push_back(entry.path());
                }
            }

            if (mFileList.size() == 0) {
                mNoInstalledThemesFound = true;    
            }

            for (std::size_t i = 0; i < mFileList.size(); i++) {
                if ((Installer::GetInstalledThemeMetadata(mFileList.at(i), &themeData)) == 0) {
                    mMenuStateFailure = true;
                    break;
                }
                // NOTE: A lot of cases where someone could have, for example, deleted the sdcafiine directory. Need to check for stuff like that
                mThemeDataList.push_back(themeData);
                mThemeNames.push_back(themeData.themeName);
            }

            if (!mMenuStateFailure & !mNoInstalledThemesFound) {
                mMenuState = MENU_STATE_DEFAULT;
                break;
            }
            else {
                if (status.trigger & VPAD_BUTTON_B) {
                    return false;
                }
            }
        case MENU_STATE_DEFAULT:
            if (status.trigger & (VPAD_STICK_L_EMULATION_UP | VPAD_BUTTON_UP)) {
                mThemeIdx--;
            }
            else if (status.trigger & (VPAD_STICK_L_EMULATION_DOWN | VPAD_BUTTON_DOWN)) {
                mThemeIdx++;
            }
            
            if (status.trigger & VPAD_BUTTON_A) {
                mSelectedThemeData = mThemeDataList.at(mThemeIdx);
                mMenuState = MENU_STATE_THEME_SHOW_DETAILS;
            }
            else if (status.trigger & VPAD_BUTTON_B) {
                return false;
            }

            if (mThemeIdx < 0) {
                mThemeIdx = mFileList.size() - 1;
            }            
            else if (mThemeIdx > static_cast<int>(mFileList.size() - 1)) {
                mThemeIdx = 0;
            }

            if (mThemeIdx < static_cast<int>(mScrollOffset)) {
                mScrollOffset = mThemeIdx;
            }
            else if (mThemeIdx >= static_cast<int>(mScrollOffset + 12)) {
                mScrollOffset = mThemeIdx - 12 + 1;
            }

            break;
        case MENU_STATE_THEME_SHOW_DETAILS:
            if (status.trigger & VPAD_BUTTON_B) {
                mMenuState = MENU_STATE_DIR_ITERATOR;
            }

            break;
                
    }

    return true;
}

std::string InstalledThemesScreen::RegionToString(std::string region)
{
    std::string regionStr;

    if (region.compare("JPN") == 0) {
        regionStr = "Japan";
    }
    else if (region.compare("USA") == 0) {
        regionStr = "America";
    }
    else if (region.compare("EUR") == 0) {
        regionStr = "Europe";
    }
    else if (region.compare("UNIVERSAL") == 0) {
        regionStr = "Universal";
    }

    return regionStr;
}
