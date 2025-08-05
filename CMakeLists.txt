cmake_minimum_required(VERSION 3.16)
project(tensorrt-cpp-api)
project(${PROJECT_NAME} VERSION 1.0.0)
set(TENSORRT_CPP_API_VERSION ${PROJECT_VERSION})

include(cmake/ccache.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)
option(DEBUG_TIME_LOGGING "Enable cout for logging runtime" TRUE)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
else()
  option(DEBUG_TIME_LOGGING "Enable cout for logging runtime" FALSE)
    set(OPTIMIZATION_FLAGS $ENV{CPP_OPTIMIZATIONS})
    set(
      CMAKE_CXX_FLAGS 
      "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS}"
    )
endif()

# Modules and dependency paths
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(TensorRT_DIR /opt/vision_dependencies/tensorrt/TensorRT-$ENV{TENSORRT_VERSION})
set(CUDA_TOOLKIT_ROOT_DIR /usr/local/cuda)

# We require CUDA, OpenCV, and TensorRT
find_package(TensorRT REQUIRED)
if(POLICY CMP0146)
  cmake_policy(SET CMP0146 OLD)
endif()
find_package(CUDA REQUIRED)
find_package(fmt REQUIRED)

set(OPENCV_VERSION $ENV{OPENCV_VERSION})
message(STATUS "OpenCV version: ${OPENCV_VERSION}")
find_package(OpenCV ${OPENCV_VERSION} REQUIRED)

# ============ LIBRARY ============

add_library(${PROJECT_NAME} STATIC
  src/engine.cpp
)

# use generators to get paths relative to the install prefix:
target_include_directories(
  ${PROJECT_NAME} PUBLIC 
  ${OpenCV_INCLUDE_DIRS} 
  ${CUDA_INCLUDE_DIRS} 
  ${TensorRT_INCLUDE_DIRS} 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  
)
target_link_libraries(
  ${PROJECT_NAME} PUBLIC 
  ${OpenCV_LIBS} ${CUDA_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${TensorRT_LIBRARIES} 
  fmt::fmt
)

add_executable(run_inference_benchmark src/main.cpp)
target_link_libraries(run_inference_benchmark ${PROJECT_NAME} fmt::fmt)

# ============ INSTALL ============
include(GNUInstallDirs)
# install compiled library or executable and to export its build information for later reuse
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)
# installs the export file (the “recipe”) created in the previous step into a standard location within your install tree (for find_package)
install(EXPORT ${PROJECT_NAME}Targets
  # NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
  PATH_VARS TensorRT_DIR CUDA_TOOLKIT_ROOT_DIR # ??
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY AnyNewerVersion
)
# build tree usage
export(EXPORT ${PROJECT_NAME}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
  # NAMESPACE ${PROJECT_NAME}::
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION lib/cmake/${PROJECT_NAME}
)
# Installs all header files (*.h, *.inl) from your include/ folder to: ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
install(DIRECTORY include/ 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.inl"
) 