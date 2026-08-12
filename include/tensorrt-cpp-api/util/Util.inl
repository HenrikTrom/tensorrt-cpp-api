#pragma once

#include <filesystem>

namespace Util {

inline bool doesFileExist(const std::string &filepath) {
    std::ifstream f(filepath.c_str());
    return f.good();
}

inline void checkCudaErrorCode(cudaError_t code) {
    if (code != cudaSuccess) {
        std::string errMsg = "CUDA operation failed with code: " + std::to_string(code) + " (" + cudaGetErrorName(code) +
                             "), with message: " + cudaGetErrorString(code);
        spdlog::error(errMsg);
        throw std::runtime_error(errMsg);
    }
}

inline bool ensureCudaDeviceAvailable() {
    try {
        const int deviceCount = cv::cuda::getCudaEnabledDeviceCount();
        if (deviceCount <= 0) {
            spdlog::error(
                "No CUDA-capable device is available. Verify 'nvidia-smi' works on the host and in the container, then recreate the container."
            );
            return false;
        }

        cv::cuda::DeviceInfo deviceInfo(0);
        spdlog::info("Using CUDA device 0: {}", deviceInfo.name());
        return true;
    } catch (const cv::Exception &e) {
        spdlog::error("Unable to query CUDA devices: {}", e.what());
        spdlog::error(
            "Verify 'nvidia-smi' works on the host and in the container before running inference."
        );
        return false;
    }
}

inline std::vector<std::string> getFilesInDirectory(const std::string &dirPath) {
        std::vector<std::string> fileNames;
        for (const auto &entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                fileNames.push_back(entry.path().string());
            }
        }
        return fileNames;
    }
}
