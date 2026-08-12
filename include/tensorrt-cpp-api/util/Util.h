#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cuda_runtime.h>
#include <opencv2/core/cuda.hpp>
#include <spdlog/spdlog.h>

namespace Util {
    // Checks if a file exists at the given file path
    bool doesFileExist(const std::string &filepath);

    // Checks and logs CUDA error codes
    void checkCudaErrorCode(cudaError_t code);

    // Checks whether a CUDA-capable device is available
    bool ensureCudaDeviceAvailable();

    // Retrieves a list of file names in the specified directory
    std::vector<std::string> getFilesInDirectory(const std::string &dirPath);
}

#include "Util.inl"
