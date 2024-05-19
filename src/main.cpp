#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/log_console.h>
#include "hips.hpp"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char **argv)
{
    WHBProcInit();
    WHBLogUdpInit();
    WHBLogConsoleInit();

    WHBLogPrintf("Hello World!");
    
    // Retrieve the paths of the input file and the patch file
    // Lil disclaimer, fs:/vol/external01/ is the path to the root of the sd card
    const char* inputPath = "fs:/vol/external01/patch_files/Men.pack";
    const char* patchPath = "fs:/vol/external01/patch_files/Men.bps";

    // Open the input file
    std::FILE* inputFile = std::fopen(inputPath, "rb");
    if (!inputFile) {
        WHBLogPrintf("Failed to open input file: %s\n", inputPath);
        return -1;
    }

    // Open the patch file
    std::FILE* patchFile = std::fopen(patchPath, "rb");
    if (!patchFile) {
        WHBLogPrintf("Failed to open patch file: %s\n", patchPath);
        std::fclose(inputFile);
        return -1;
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
        return -1;
    }

    if (std::fread(patchData.data(), 1, patchSize, patchFile) != patchSize) {
        WHBLogPrintf("Failed to read patch file.\n");
        std::fclose(inputFile);
        std::fclose(patchFile);
        return -1;
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
        return -1;
    }

    auto [bytes, result] = Hips::patch(inputData.data(), inputSize, patchData.data(), patchSize, patchType);
    if (result == Hips::Result::Success) {
        WHBLogPrintf("Patch applied successfully\n");

        // Write the patch file and test it out haha
        std::FILE* outputFile = std::fopen("fs:/vol/external01/patch_files/men_packed.pack", "wb");
        if (!outputFile) {
            WHBLogPrintf("Failed to open output file: men_packed.pack\n");
            return -1;
        }

        if (std::fwrite(bytes.data(), 1, bytes.size(), outputFile) != bytes.size()) {
            WHBLogPrintf("Failed to write to output file: men_packed.pack\n");
            std::fclose(outputFile);
            return -1;
        }

        std::fclose(outputFile);
    } else {
        WHBLogPrintf("Patching failed :(\n");
    }

    while (WHBProcIsRunning()) {
        WHBLogConsoleDraw();
    }

    WHBLogConsoleFree();
    WHBLogUdpDeinit();
    WHBProcShutdown();

    return 0;
}