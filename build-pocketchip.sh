#!/bin/bash
# Build script for Pocket CHIP
# This script should be run directly on the Pocket CHIP device

set -e  # Exit on error

echo "==================================="
echo "Lode Runner 2020 - Pocket CHIP Build"
echo "==================================="
echo ""

# Check if running on Pocket CHIP
if [ ! -f /etc/apt/sources.list.d/chip.list ] && [ ! -f /usr/bin/pocketchip-load ]; then
    echo "WARNING: This doesn't appear to be a Pocket CHIP system."
    echo "Continue anyway? (y/n)"
    read -r response
    if [ "$response" != "y" ]; then
        exit 1
    fi
fi

# Install dependencies
echo "Installing dependencies..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libasound2-dev \
    libgles2-mesa-dev \
    libegl1-mesa-dev \
    libglfw3-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libxxf86vm-dev

echo ""
echo "Dependencies installed successfully!"
echo ""

# Create build directory
BUILD_DIR="build-pocketchip"
echo "Creating build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Configure with CMake
echo ""
echo "Configuring build for Pocket CHIP..."
cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DPOCKET_CHIP=ON \
    -DVIDEO_RECORDING=OFF \
    -DCMAKE_CXX_FLAGS="-march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=hard -O2 -ffast-math"

# Build
echo ""
echo "Building Lode Runner..."
cmake --build "$BUILD_DIR" -j2  # Use 2 cores (Pocket CHIP has limited resources)

echo ""
echo "==================================="
echo "Build completed successfully!"
echo "==================================="
echo ""
echo "To install the game, run:"
echo "  sudo cmake --install $BUILD_DIR --prefix /usr/local/games/loderunner"
echo ""
echo "Or to run directly from the build directory:"
echo "  cd $BUILD_DIR && ./LodeRunner"
echo ""
