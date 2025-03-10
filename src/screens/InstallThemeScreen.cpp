#include <filesystem>

#include <coreinit/time.h>
#include <coreinit/thread.h>

#include "gfx.h"
#include "installer.h"
#include "InstallThemeScreen.h"
#include "utils.h"

InstallThemeScreen::InstallThemeScreen()
    :   mMenuState(MENU_STATE_INIT),
        mMenuStateFailure(false),
        mFileIdx(0),
        mScrollOffset(0)
{
}

InstallThemeScreen::~InstallThemeScreen()
{
}

void InstallThemeScreen::Draw()
{
    switch (mMenuState) {
        case MENU_STATE_INIT:
            break;
        case MENU_STATE_DIR_ITERATOR:
            if (mMenuStateFailure) {
                Gfx::SetBackgroundColour(BACKGROUND_ERR_COLOUR);
                Gfx::Print(-4, 2, "Could not find any themes in:\n%s\n\nPlease ensure you have your theme files in the correct place.", THEMES_ROOT);
                Gfx::Print(-4, 17, "                             B - Back");
                break;
            }
            
            Gfx::SetBackgroundColour(BACKGROUND_COLOUR);
            Gfx::Print(-4, 2, "Iterating through\n%s...", THEMES_ROOT);
            break;
        case MENU_STATE_DEFAULT: {
            Gfx::Print(-3, 2, "Select a theme to install");
            
            int yIni = 4;
            for (std::size_t i = 0; i < 12 && (mScrollOffset + i) < mFileList.size(); i++) {
                int y = yIni + i;
                std::size_t fileIdx = mScrollOffset + i;
                Gfx::Print(-4, y, (mFileIdx == static_cast<int>(fileIdx)) ? "> %s" : "  %s", mFileList.at(fileIdx).c_str());
            }

            Gfx::Print(-4, 17, "A - Select                D-Pad/Stick - Move               B - Back");
            break;
        }
        case MENU_STATE_INSTALL_THEME_PROMPT:
            if (!mMenuStateFailure & !mThemeRegionMismatch & !mThemeAlreadyInstalled) {
                Gfx::SetBackgroundColour(BACKGROUND_COLOUR);
                Gfx::Print(-4, 2, "You will now install: %s\nas an SDCafiine modpack.\n\nWould you like to continue with the installation?", mThemeName.c_str());
                Gfx::Print(-4, 17, "            A - Yes                              B - No");
            }
            else if (mThemeAlreadyInstalled) {
                Gfx::SetBackgroundColour(BACKGROUND_WARNING_COLOUR);
                Gfx::Print(-4, 2, "Warning!\n\nThis theme is already installed as the modpack:\n%s\n\nWould you like to reinstall it?", mThemeID.c_str());
                Gfx::Print(-4, 17, "            A - Yes                              B - No");
            }            
            else if (mThemeRegionMismatch) {
                Gfx::SetBackgroundColour(BACKGROUND_WARNING_COLOUR);
                Gfx::Print(-4, 2, "Warning!\n\nThis theme's region (%s) does not match your\nWii U Menu's region (%s).\n\nIf this theme contains any text patches for specific languages,\nthe theme will fail to install.\n\nWould you like to continue with the installation of:\n%s?", RegionToString(mThemeRegion).c_str(), RegionToString(mSystemRegion).c_str(), mThemeName.c_str());
                Gfx::Print(-4, 17, "            A - Yes                              B - No");
            }
            else {
                Gfx::SetBackgroundColour(BACKGROUND_ERR_COLOUR);
                Gfx::Print(-4, 2, "There was an error reading the metadata for:\n%s\n\nThere may be a problem with the theme...", mSelectedPathShort.c_str());
                Gfx::Print(-4, 17, "                         B - Back");
            }
            break;
        case MENU_STATE_INSTALLING_THEME:
            Gfx::SetBackgroundColour(BACKGROUND_COLOUR);
            Gfx::Print(-4, 2, "Installing %s by %s...\nThis may take time, do not turn of your Wii U.", mThemeName.c_str(), mThemeAuthor.c_str());
            break;
        case MENU_STATE_THEME_INSTALL_SUCCESS:
            Gfx::SetBackgroundColour(BACKGROUND_SUCCESS_COLOUR);
            Gfx::Print(-4, 2, "Successfully installed %s!\n\nModpack name: %s", mThemeName.c_str(), mThemeID.c_str());
            Gfx::Print(-4, 17, "                          A/B - Continue");
            break;
        case MENU_STATE_THEME_INSTALL_ERROR:
            Gfx::SetBackgroundColour(BACKGROUND_ERR_COLOUR);
            Gfx::Print(-4, 2, "Error installing %s from the file %s!\n\nThere may be a problem with the theme...", mThemeName.c_str(), mSelectedPathShort.c_str());
            Gfx::Print(-4, 17, "                             B - Back");
            break;
    }
}

bool InstallThemeScreen::Update(VPADStatus status)
{    
    switch(mMenuState) {
        case MENU_STATE_INIT:
            std::filesystem::create_directories(THEMES_ROOT);

            mSystemRegion = Installer::GetSystemRegion();

            mMenuState = MENU_STATE_DIR_ITERATOR;
            break;
        case MENU_STATE_DIR_ITERATOR:
            mFileList.clear();
            mFileListFullPath.clear();
            
            for (const auto & entry : std::filesystem::directory_iterator(THEMES_ROOT)) {
                if (entry.path().extension() == ".utheme") {
                    mFileList.push_back(entry.path().filename());
                    mFileListFullPath.push_back(entry.path());
                }
            }

            if (mFileList.size() == 0) {
                mMenuStateFailure = true;
            }

            if (!mMenuStateFailure) {
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
                mFileIdx--;
            }
            else if (status.trigger & (VPAD_STICK_L_EMULATION_DOWN | VPAD_BUTTON_DOWN)) {
                mFileIdx++;
            }

            if (status.trigger & VPAD_BUTTON_A) {
                mSelectedPath = mFileListFullPath.at(mFileIdx);
                mSelectedPathShort = mFileList.at(mFileIdx);
                mMenuState = MENU_STATE_INSTALL_THEME_PROMPT;
            }
            else if (status.trigger & VPAD_BUTTON_B) {
                return false;
            }

            if (mFileIdx < 0) {
                mFileIdx = mFileList.size() - 1;
            }            
            else if (mFileIdx > static_cast<int>(mFileList.size() - 1)) {
                mFileIdx = 0;
            }

            if (mFileIdx < static_cast<int>(mScrollOffset)) {
                mScrollOffset = mFileIdx;
            }
            else if (mFileIdx >= static_cast<int>(mScrollOffset + 12)) {
                mScrollOffset = mFileIdx - 12 + 1;
            }

            break;
        case MENU_STATE_INSTALL_THEME_PROMPT:
            if ((Installer::GetThemeMetadata(mSelectedPath, &mThemeData)) == 0) {
                mMenuStateFailure = true;
            }

            mThemeAlreadyInstalled = false;
            mThemeRegionMismatch = false;

            mThemeID = mThemeData.themeID;
            mThemeName = mThemeData.themeName;
            mThemeAuthor = mThemeData.themeAuthor;
            mThemeRegion = mThemeData.themeRegion;

            if ((mThemeRegion != mSystemRegion) & (mThemeRegion != Installer::Region::UNIVERSAL)) {
                mThemeRegionMismatch = true;
            }

            if (IsThemeAlreadyInstalled()) {
                mThemeAlreadyInstalled = true;
            }
            
            if (!mMenuStateFailure | mThemeRegionMismatch | mThemeAlreadyInstalled) {
                if (status.trigger & VPAD_BUTTON_A) {
                    Gfx::SetBackgroundColour(BACKGROUND_COLOUR);
                    mMenuState = MENU_STATE_INSTALLING_THEME;
                }
                else if (status.trigger & VPAD_BUTTON_B) {
                    mMenuState = MENU_STATE_DIR_ITERATOR;
                }
            }
            else {
                if (status.trigger & VPAD_BUTTON_B) {
                    mMenuState = MENU_STATE_DIR_ITERATOR;
                }
            }
            
            break;
        case MENU_STATE_INSTALLING_THEME:
            if (!Installer::InstallTheme(mSelectedPath, mThemeData)) {
                mMenuState = MENU_STATE_THEME_INSTALL_ERROR;
                break;
            }

            mMenuState = MENU_STATE_THEME_INSTALL_SUCCESS;
            break;
        case MENU_STATE_THEME_INSTALL_SUCCESS:
            if (status.trigger & (VPAD_BUTTON_A | VPAD_BUTTON_B)) {
                mMenuState = MENU_STATE_DIR_ITERATOR;
            }
            break;
        case MENU_STATE_THEME_INSTALL_ERROR:
            if (status.trigger & VPAD_BUTTON_B) {
                mMenuState = MENU_STATE_DIR_ITERATOR;
            }
            break;
            
    }

    return true;
}

std::string InstallThemeScreen::RegionToString(Installer::Region region)
{
    std::string regionStr;

    switch (region) {
        case Installer::Region::JPN:
            regionStr = "Japan";
            break;
        case Installer::Region::USA:
            regionStr = "America";
            break;
        case Installer::Region::EUR:
            regionStr = "Europe";
            break;
        default:
            regionStr = "";
            break;
    }

    return regionStr;
}

bool InstallThemeScreen::IsThemeAlreadyInstalled()
{
    if (!mThemeAlreadyInstalled) {
        std::string themeInstallPath = std::string(THEMIIFY_INSTALLED_THEMES) + "/" + mThemeID + ".json";

        std::FILE *themeInstallFile;
        // For now just gonna do it like this
        // Maybe in the future we could actually parse the json for the install path and check if that exists on the sd
        if (!(themeInstallFile = std::fopen(themeInstallPath.c_str(), "rb"))) {
            std::fclose(themeInstallFile);
            return false;
        }

        std::fclose(themeInstallFile);
        return true;
    }

    return true;
}