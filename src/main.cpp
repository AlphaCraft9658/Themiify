#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/log_console.h>
#include <sysapp/title.h>
#include <mocha/mocha.h>
#include <sysapp/launch.h>
#include <vpad/input.h>
#include <padscore/kpad.h>
#include <utils/DrawUtils.h>
#include <nlohmann/json.hpp>
#include <hips.hpp>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include <zip.h>
#include <ui.h>
using namespace std::literals;
namespace json = nlohmann;

typedef enum zipStatCheckedStatus {
    ZIP_STAT_CHECKED_FAILURE,
    ZIP_STAT_CHECKED_SUCCESS
} zipStatCheckedStatus;

int error(std::string errorMessage="") {
    WHBLogPrintf(errorMessage.c_str());
    // WHBLogConsoleDraw();
    SYSLaunchMenu();
    while (WHBProcIsRunning()) {}
    DrawUtils::DeInit();
    Mocha_UnmountFS("storage_mlc");
    VPADShutdown();
    KPADShutdown();
    // WHBLogConsoleFree();
    Mocha_DeInitLibrary();
    WHBLogUdpDeinit();
    WHBProcShutdown();
    return -1;
}

bool create_filepath(std::string filepath) {
        return std::filesystem::create_directories(filepath);
}

// trim from end (in place)
inline void rtrim_path(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return ch == '/';
    }).base(), s.end());
}

// trim from end (copying)
inline std::string rtrim_path_cp(std::string s) {
    rtrim_path(s);
    return s;
}

// Creates the parent directories of a file
bool create_parent_directory_structure(std::string filepath) {
    return create_filepath(rtrim_path_cp(filepath));
}

std::vector<std::string> listZipEntries(zip* zipFile) {
    std::vector<std::string> result;
    for (int i=0; i < zip_get_num_entries(zipFile, 0); i++) {
        result.push_back(zip_get_name(zipFile, i, 0));
    }
    return result;
}

zipStatCheckedStatus zip_stat_checked(zip_t* zipFile, const char* fileName, zip_flags_t zipFlags, zip_stat_t* statObject, int zipErrorInt) {
    if (zip_stat(zipFile, fileName, zipFlags, statObject) != 0) {
        zip_error_t error_type;
        zip_error_init_with_code(&error_type, zipErrorInt);
        error("Could not stat file \"" + std::string(fileName) + "\": " + std::string(zip_error_strerror(&error_type)) + " | (bad theme?)");
        return ZIP_STAT_CHECKED_FAILURE;
    }
    return ZIP_STAT_CHECKED_SUCCESS;
}

int main(int argc, char **argv)
{
    WHBProcInit();
    WHBLogUdpInit();
    // WHBLogConsoleInit();
    // WHBLogConsoleSetColor(0x000000);
    if (Mocha_InitLibrary() != MOCHA_RESULT_SUCCESS) {
        return error("Mocha library did not initialize properly");
    }
    VPADInit();
    KPADInit();
    OSEnableHomeButtonMenu(false);
    if (!DrawUtils::Init()) {
        return error("Could not initialize DrawUtils");
    }
    DrawUtils::initFont();
    DrawUtils::setFontColor(Color(COLOR_WHITE));
    DrawUtils::setFontSize(24);

    std::unique_ptr<ui::Menu> menu = std::make_unique<ui::Menu>("Themiify - Development Build");
    DrawUtils::beginDraw();
    DrawUtils::clear(Color(COLOR_BLACK));
    menu->render_header();
    DrawUtils::endDraw();

    std::unique_ptr<VPADStatus> vpadStatus = std::make_unique<VPADStatus>();
    std::unique_ptr<KPADStatus> kpadStatus = std::make_unique<KPADStatus>();

    // Mount storage_mlc
    // NOTE: storage_mlc is the system mlc_storage where the system menu as well as other system titles are located
    //       it could be risky when not handled properly, because wrong commands may override important system files.
    Mocha_MountFS("storage_mlc", nullptr, "/vol/storage_mlc01");

    uint64_t menuID = _SYSGetSystemApplicationTitleId(SYSTEM_APP_ID_WII_U_MENU);  // Retrieve the Wii U system menu titleID
    
    uint32_t menuIDParentDir = (uint32_t)(menuID >> 32);
    uint32_t menuIDChildDir = (uint32_t)menuID;

    char menuIDStr[17];
    snprintf(menuIDStr, sizeof(menuIDStr), "%016llx", menuID);

    char splitMenuID[18];
    snprintf(splitMenuID, sizeof(splitMenuID), "%08x/%08x", menuIDParentDir, menuIDChildDir);

    const std::string modpacksRoot = "fs:/vol/external01/wiiu/sdcafiine/" + std::string(menuIDStr) + "/";  // Will later be used for getting modpack paths, which depend on the theme name
    const std::string modPath = modpacksRoot + "themiify/";
    const std::string menuContentPath = "storage_mlc:/sys/title/" + std::string(splitMenuID) + "/content/";

    // Themiify resource directories
    const std::string themiifyRoot = "fs:/vol/external01/wiiu/themiify/";
    const std::string themiifyTmp = themiifyRoot + "tmp/";
    create_filepath(themiifyTmp);
    const std::string themesPath = themiifyRoot + "themes/";

    std::string themeID;
    std::string themePath = themesPath + "test_theme" + "/";

    // I'm great at menu design :D - Nathaniel
    WHBLogPrintf("Themiify");
    WHBLogPrintf("-----------------------------------------------------");
    WHBLogPrintf("Installed Wii U Menu title ID: %s", menuIDStr);
    // WHBLogConsoleDraw();

    struct zip* themeArchive;
    int zipErr;

    if ((themeArchive = zip_open((themesPath + "test_theme.utheme").c_str(), ZIP_RDONLY, &zipErr)) == NULL) {
        zip_error_t error_type;
        zip_error_init_with_code(&error_type, zipErr);
        return error("Cannot open zip archive:" + std::string(zip_error_strerror(&error_type)));
    }
    
    WHBLogPrintf("----- Reading metadata from archive -----");
    struct zip_file* metadataFile;
    if (zip_name_locate(themeArchive, "metadata.json", 0) == -1) {
        return error("Could not locate metadata.json file inside of the archive");
    }

    metadataFile = zip_fopen(themeArchive, "metadata.json", 0);

    struct zip_stat metaStat;
    if (zip_stat_checked(themeArchive, "metadata.json", ZIP_STAT_SIZE, &metaStat, zipErr) == ZIP_STAT_CHECKED_FAILURE) {
        return -1;
    }

    std::string buffer(metaStat.size, '\0');

    zip_fread(metadataFile, &buffer[0], metaStat.size);
    zip_fclose(metadataFile);

    std::unique_ptr<json::json> themeMeta = std::make_unique<json::json>(json::json::parse(buffer));

    themeID = std::string(themeMeta->at("Metadata").at("themeID"));

    WHBLogPrintf("----- Theme Info -----");
    WHBLogPrintf("Theme Name: %s", std::string(themeMeta->at("Metadata").at("themeName")).c_str());
    WHBLogPrintf("Theme Author: %s", std::string(themeMeta->at("Metadata").at("themeAuthor")).c_str());
    WHBLogPrintf("Theme ID: %s", std::string(themeMeta->at("Metadata").at("themeID")).c_str());

    // List included patches
    WHBLogPrintf("----- Included Patches -----");
    for (auto& item : themeMeta->at("Patches").items()) {
        WHBLogPrintf(std::string(item.value()).c_str());
    }

    WHBLogPrintf("----- Starting Patch -----");
    // WHBLogConsoleDraw();

    // Patch all files listed in the "Patches" section of the theme metadata
    for (auto& [patchFilepath, menuFilePath] : themeMeta->at("Patches").items()) {
        // Generate the paths of the input file, the patch file and the output file
        std::string inputPath = menuContentPath + std::string(menuFilePath);
        std::string patchPath = "patches/" + patchFilepath;
        std::string outputPath = modPath + "content/" + std::string(menuFilePath);
        
        // Prepare parent directory structure required for currently patched file
        create_parent_directory_structure(outputPath);
        WHBLogPrintf("Filepath created");
        // WHBLogConsoleDraw();

        WHBLogPrintf(("Applying patch: " + patchFilepath).c_str());
        // WHBLogConsoleDraw();


        // Open input file
        std::FILE* inputFile = std::fopen(inputPath.c_str(), "rb");
        if (!inputFile) {
            return error("Failed to open input file: " + inputPath);
        }

        // Open patch file
        struct zip_file* patchFile = zip_fopen(themeArchive, patchPath.c_str(), 0);
        if (patchFile == NULL) {
            zip_error_t error_type;
            zip_error_init_with_code(&error_type, zipErr);
            return error("Could not open patch file " + patchPath + ":" + std::string(zip_error_strerror(&error_type)));
        }

        // Get the size of the input file
        std::fseek(inputFile, 0, SEEK_END);
        std::size_t inputSize = std::ftell(inputFile);
        std::rewind(inputFile);

        // Get the size of the patch file
        struct zip_stat patchStat;
        if (zip_stat_checked(themeArchive, patchPath.c_str(), ZIP_STAT_SIZE, &patchStat, zipErr) == ZIP_STAT_CHECKED_FAILURE) {
            return -1;
        }
        std::size_t patchSize = patchStat.size;

        // Read the files into arrays
        std::vector<uint8_t> inputData(inputSize);
        std::vector<uint8_t> patchData(patchSize);

        if (std::fread(inputData.data(), 1, inputSize, inputFile) != inputSize) {
            std::fclose(inputFile);
            zip_fclose(patchFile);
            return error("Failed to read input file.");
        }

        if (zip_fread(patchFile, patchData.data(), patchSize) != patchSize) {
            std::fclose(inputFile);
            zip_fclose(patchFile);
            return error("Failed to read patch file.");
        }

        std::fclose(inputFile);
        zip_fclose(patchFile);

        WHBLogPrintf("Patching file, please wait...");
        // WHBLogConsoleDraw();

        auto [bytes, result] = Hips::patch(inputData.data(), inputSize, patchData.data(), patchSize, Hips::PatchType::BPS);
        if (result == Hips::Result::Success) {
            WHBLogPrintf("Patch applied successfully");
            WHBLogPrintf("Writing file, please wait...");
            WHBLogPrintf("(Your console isn't frozen in this case!)");
            // WHBLogConsoleDraw();

            // Write the patch file and test it out haha
            std::FILE* outputFile = std::fopen(outputPath.c_str(), "wb");
            if (!outputFile) {
                return error("Failed to open output file: " + outputPath);
            }

            if (std::fwrite(bytes.data(), 1, bytes.size(), outputFile) != bytes.size()) {
                std::fclose(outputFile);
                return error("Failed to write to output file (an incorrect number of bytes has been written): " + outputPath);
            }

            std::fclose(outputFile);
            WHBLogPrintf("----------");
            WHBLogPrintf(("Successfully applied patch: " + patchFilepath).c_str());
            WHBLogPrintf("");
        } else {
            return error("Patching failed :(");
        }
    }

    WHBLogPrintf("Patch succeeded! (Press A to close)");
    zip_close(themeArchive);
    bool closed = false;
    while (WHBProcIsRunning()) {
        VPADRead(VPAD_CHAN_0, vpadStatus.get(), 1, NULL);
        KPADRead(WPAD_CHAN_0, kpadStatus.get(), 1);
        if ((vpadStatus->trigger == VPAD_BUTTON_A || kpadStatus->trigger == WPAD_PRO_BUTTON_A) && !closed) SYSLaunchMenu();
        // WHBLogConsoleDraw();
    }

    DrawUtils::DeInit();
    Mocha_UnmountFS("storage_mlc");
    VPADShutdown();
    KPADShutdown();
    // WHBLogConsoleFree();
    Mocha_DeInitLibrary();
    WHBLogUdpDeinit();
    WHBProcShutdown();

    return 0;
}