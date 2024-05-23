#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/log_console.h>
#include <sysapp/title.h>
#include <mocha/mocha.h>
#include "hips.hpp"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

int error() {
    while (WHBProcIsRunning()) {
            WHBLogConsoleDraw();
    }
    Mocha_UnmountFS("storage_mlc");
    WHBLogConsoleFree();
    WHBLogUdpDeinit();
    WHBProcShutdown();
    return -1;
}

int main(int argc, char **argv)
{
    WHBProcInit();
    WHBLogUdpInit();
    WHBLogConsoleInit();
    Mocha_InitLibrary();

    // Mount storage_mlc
    // NOTE: storage_mlc is the system mlc_storage where the system menu as well as other system titles are located
    //       it could be risky when not handled properly, because wrong commands may override important system files.
    Mocha_MountFS("storage_mlc", "/dev/mlc01", "/vol/storage_mlc");

    uint64_t menuID = _SYSGetSystemApplicationTitleId(SYSTEM_APP_ID_WII_U_MENU);  // Retrieve the Wii U system menu titleID
    // Convert the uint64_t menuID into a char array
    char menuIDString[17];
    snprintf(menuIDString, 17, "%016llx", menuID);  // Write the titleID of the Wii U system menu to a string/character array

    // Generate and store the path of the content directory of the Wii U system menu both for sdcafiine and the system
    std::string sdcafiineContentFolder = menuIDString;
    sdcafiineContentFolder = "fs:/vol/external01/wiiu/sdcafiine/" + sdcafiineContentFolder.substr(0, 8) + "/" + sdcafiineContentFolder.substr(8, 8) + "/content";
    std::string systemContentFolder = menuIDString;
    systemContentFolder = "storage_mlc:/sys/title/" + systemContentFolder.substr(0, 8) + "/" + systemContentFolder.substr(8, 8) + "/content";

    // Print those directories and the menu titleID for debugging purposes
    WHBLogPrintf("Wii U system menu titleID: %016hhs", menuIDString);
    WHBLogPrintf(sdcafiineContentFolder.c_str());
    WHBLogPrintf(systemContentFolder.c_str());


    // Retrieve the paths of the input file and the patch file
    // Lil disclaimer, fs:/vol/external01/ is the path to the root of the sd card
    // const char* internalMenPath = (systemContentFolder + "/Common/Package/Men.pack").c_str();
    // const char* inputPath = "fs:/vol/external01/patch_files/Men.pack";
    // const char* inputPath = std::string(systemContentFolder + "/Common/Package/Men.pack").c_str();
    std::string inputPath = systemContentFolder + "/Common/Package/Men.pack";
    std::string patchPath = "fs:/vol/external01/patch_files/Men.bps";
    std::string inputPath2 = systemContentFolder + "/Common/Package/Men2.pack";
    std::string patchPath2 = "fs:/vol/external01/patch_files/Men2.bps";

    // Open input file 1
    std::FILE* inputFile = std::fopen(inputPath.c_str(), "rb");
    if (!inputFile) {
        WHBLogPrintf("Failed to open input file: %s\n", inputPath.c_str());
        return error();
    }

    // Open input file 2
    std::FILE* inputFile2 = std::fopen(inputPath2.c_str(), "rb");
    if (!inputFile2) {
        WHBLogPrintf("Failed to open input file: %s\n", inputPath2.c_str());
        std::fclose(inputFile);
        return error();
    }

    // Open patch file 1
    std::FILE* patchFile = std::fopen(patchPath.c_str(), "rb");
    if (!patchFile) {
        WHBLogPrintf("Failed to open patch file: %s\n", patchPath.c_str());
        std::fclose(inputFile);
        std::fclose(inputFile2);
        return error();
    }

    // Open patch file 2
    std::FILE* patchFile2 = std::fopen(patchPath2.c_str(), "rb");
    if (!patchFile2) {
        WHBLogPrintf("Failed to open patch file: %s\n", patchPath2.c_str());
        std::fclose(inputFile);
        std::fclose(inputFile2);
        std::fclose(patchFile);
        return error();
    }

    // Get the size of the input file
    std::fseek(inputFile, 0, SEEK_END);
    std::size_t inputSize = std::ftell(inputFile);
    std::rewind(inputFile);

    // Get the size of the patch file
    std::fseek(patchFile, 0, SEEK_END);
    std::size_t patchSize = std::ftell(patchFile);
    std::rewind(patchFile);

    // Read the files into arrays
    std::vector<uint8_t> inputData(inputSize);
    std::vector<uint8_t> patchData(patchSize);

    if (std::fread(inputData.data(), 1, inputSize, inputFile) != inputSize) {
        WHBLogPrintf("Failed to read input file.\n");
        std::fclose(inputFile);
        std::fclose(patchFile);
        return error();
    }

    if (std::fread(patchData.data(), 1, patchSize, patchFile) != patchSize) {
        WHBLogPrintf("Failed to read patch file.\n");
        std::fclose(inputFile);
        std::fclose(patchFile);
        return error();
    }

    std::fclose(inputFile);
    std::fclose(patchFile);

    Hips::PatchType patchType;
    std::string extension = patchPath;
    // This stuff isn't necesarry cuz it'll always be bps but rn I totally just copied this from the og example lmfao
    if (extension.size() >= 4 && extension.substr(extension.size() - 4) == ".ips") {
        patchType = Hips::PatchType::IPS;
    } else if (extension.size() >= 4 && extension.substr(extension.size() - 4) == ".ups") {
        patchType = Hips::PatchType::UPS;
    } else if (extension.size() >= 4 && extension.substr(extension.size() - 4) == ".bps") {
        patchType = Hips::PatchType::BPS;
    } else {
        WHBLogPrintf("Unknown patch format\n");
        return error();
    }

    auto [bytes, result] = Hips::patch(inputData.data(), inputSize, patchData.data(), patchSize, patchType);
    if (result == Hips::Result::Success) {
        WHBLogPrintf("Patch applied successfully\n");
        WHBLogPrintf("Writing file, please wait...");
        WHBLogPrintf("(your console isn't frozen in this case!)");
        WHBLogConsoleDraw();

        // Write the patch file and test it out haha
        std::FILE* outputFile = std::fopen("fs:/vol/external01/patch_files/Men-patched.pack", "wb");
        if (!outputFile) {
            WHBLogPrintf("Failed to open output file: Men-patched.pack\n");
            return error();
        }

        if (std::fwrite(bytes.data(), 1, bytes.size(), outputFile) != bytes.size()) {
            WHBLogPrintf("Failed to write to output file: Men-patched.pack\n");
            std::fclose(outputFile);
            return error();
        }

        std::fclose(outputFile);
        WHBLogPrintf("Done!");
    } else {
        WHBLogPrintf("Patching failed :(\n");
    }

    while (WHBProcIsRunning()) {
        WHBLogConsoleDraw();
    }

    Mocha_UnmountFS("storage_mlc");
    WHBLogConsoleFree();
    WHBLogUdpDeinit();
    WHBProcShutdown();

    return 0;
}