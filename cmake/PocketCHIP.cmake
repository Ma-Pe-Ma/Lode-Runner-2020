# Toolchain file for Pocket CHIP
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/PocketCHIP.cmake -DPOCKET_CHIP=ON -B build-pocketchip

# Set target system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7l)

# Define Pocket CHIP specific options
set(POCKET_CHIP ON CACHE BOOL "Build for Pocket CHIP")

# Compiler settings (use native compiler on Pocket CHIP)
# If cross-compiling from another machine, set these paths:
# set(CMAKE_C_COMPILER /path/to/arm-linux-gnueabihf-gcc)
# set(CMAKE_CXX_COMPILER /path/to/arm-linux-gnueabihf-g++)

# OpenGL ES settings for Mali-400
set(OPENGL_ES ON CACHE BOOL "Use OpenGL ES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPOCKET_CHIP")

# Optimization flags for ARM Cortex-A8
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe -ffast-math")

# Reduce memory usage
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_DYNAMIC_ARRAY")

# Find packages hint paths (adjust if needed)
set(CMAKE_PREFIX_PATH /usr/local /usr)

message(STATUS "Configuring for Pocket CHIP (ARM Cortex-A8 + Mali-400)")
