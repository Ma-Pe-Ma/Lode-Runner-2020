# Pocket CHIP Port - Summary of Changes

This document summarizes all modifications made to port Lode Runner 2020 to the Pocket CHIP handheld device.

## Overview

The Pocket CHIP port enables this game to run on the Pocket CHIP handheld computer (ARM Cortex-A8, Mali-400 GPU, 512MB RAM, 480x272 display).

## Files Added

### 1. `cmake/PocketCHIP.cmake`
- CMake toolchain file for Pocket CHIP builds
- Sets ARM compiler flags and optimization settings
- Defines POCKET_CHIP build flag

### 2. `assets/config-pocketchip.json`
- Configuration file with 480x272 native resolution
- Optimized settings for Pocket CHIP hardware

### 3. `build-pocketchip.sh`
- Automated build script for Pocket CHIP
- Installs dependencies and configures build
- Can be run directly on the Pocket CHIP device

### 4. `POCKETCHIP.md`
- Comprehensive documentation for Pocket CHIP users
- Build instructions, controls, troubleshooting
- Performance optimization tips

### 5. `POCKETCHIP-PORT-SUMMARY.md` (this file)
- Technical summary of all port changes

## Files Modified

### 1. `CMakeLists.txt`
**Location**: Lines 197-221 (after UNIX section)

**Changes**:
- Added Pocket CHIP detection and configuration block
- Configures OpenGL ES 3.0 support (IMGUI_IMPL_OPENGL_ES3)
- Links GLESv2 and EGL libraries
- Disables video recording for resource-constrained device

**Code Added**:
```cmake
# Pocket CHIP specific configuration
if(POCKET_CHIP)
    message(STATUS "Configuring for Pocket CHIP")
    target_compile_definitions(LodeRunner PRIVATE
        -DPOCKET_CHIP
        -DIMGUI_IMPL_OPENGL_ES3
    )
    find_package(OpenGLES2 QUIET)
    if(OpenGLES2_FOUND)
        target_link_libraries(LodeRunner PRIVATE ${OPENGLES2_LIBRARIES})
        target_include_directories(LodeRunner PRIVATE ${OPENGLES2_INCLUDE_DIRS})
    else()
        target_link_libraries(LodeRunner PRIVATE GLESv2 EGL)
    endif()
    set(VIDEO_RECORDING OFF CACHE BOOL "Video recording disabled for Pocket CHIP" FORCE)
    message(STATUS "Pocket CHIP optimizations enabled")
endif()
```

### 2. `src/loderunner/iocontext/GlfwIOContext.cpp`

**Changes**:

#### Include Section (Lines 9-16)
- Added POCKET_CHIP to OpenGL ES conditional compilation
- Includes GLES3/gl3.h for Pocket CHIP builds

**Before**:
```cpp
#if defined __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
```

**After**:
```cpp
#if defined __EMSCRIPTEN__ || defined POCKET_CHIP
#include <GLES3/gl3.h>
#if defined __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#else
#include <glad/glad.h>
#endif
```

#### GLFW Window Hints (Lines 314-327)
- Configures OpenGL ES 3.0 context for Pocket CHIP

**Modified**:
```cpp
#if defined __EMSCRIPTEN__ || defined POCKET_CHIP
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
```

#### GLAD Loader (Lines 379-386)
- Skips GLAD loading on Pocket CHIP (uses native OpenGL ES)

**Modified**:
```cpp
#if !defined __EMSCRIPTEN__ && !defined POCKET_CHIP
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        int a;
        std::cin >> a;
        return;
    }
#endif
```

#### ImGui OpenGL Initialization (Lines 409-413)
- Uses GLSL ES 300 shader version for Pocket CHIP

**Modified**:
```cpp
#if defined __EMSCRIPTEN__ || defined POCKET_CHIP
    ImGui_ImplOpenGL3_Init("#version 300 es");
#else
    ImGui_ImplOpenGL3_Init("#version 460 core");
#endif
```

### 3. `src/loderunner/iocontext/rendering/Shader.h`

**Changes**:

#### Include Section (Lines 4-11)
- Added POCKET_CHIP to OpenGL ES header includes

**Modified**:
```cpp
#if defined __EMSCRIPTEN__ || defined POCKET_CHIP
#include <GLES3/gl3.h>
#if defined __EMSCRIPTEN__
#include "emscripten.h"
#endif
#else
#include <glad/glad.h>
#endif
```

#### Shader Version Selection (Lines 82-89)
- Uses GLSL ES 300 shaders for Pocket CHIP

**Modified**:
```cpp
#if defined __EMSCRIPTEN__ || defined ANDROID_VERSION || defined POCKET_CHIP
    const GLchar* vertex_shader_with_version[2] = { "#version 300 es\nprecision highp float;\n", vertexCode.c_str() };
    const GLchar* fragment_shader_with_version[2] = { "#version 300 es\nprecision highp float;\n", fragmentCode.c_str() };
#else
    const GLchar* vertex_shader_with_version[2] = { "#version 460 core\n", vertexCode.c_str() };
    const GLchar* fragment_shader_with_version[2] = { "#version 460 core\n", fragmentCode.c_str() };
#endif
```

### 4. `ReadMe.md`

**Changes**:
- Added Pocket CHIP compatibility badge
- Added link to POCKETCHIP.md documentation

## Technical Implementation Details

### OpenGL ES 3.0 Support
- Game already had OpenGL ES support for Emscripten (WebAssembly)
- Extended existing conditionals to include POCKET_CHIP flag
- Mali-400 GPU supports OpenGL ES 2.0/3.0 (uses ES 3.0)

### Shader Compatibility
- All shaders already written to be ES 3.0 compatible
- Uses `#version 300 es` with `precision highp float`
- No shader modifications required

### Resolution Handling
- Game has flexible resolution system via config.json
- Created 480x272 preset for Pocket CHIP's native display
- Viewport calculations handle aspect ratio automatically

### Performance Optimizations
- ARM Cortex-A8 specific compiler flags:
  - `-march=armv7-a` - ARMv7 architecture
  - `-mtune=cortex-a8` - Cortex-A8 CPU tuning
  - `-mfpu=neon` - NEON SIMD instructions
  - `-mfloat-abi=hard` - Hardware floating point
  - `-O2` - Optimization level 2
  - `-ffast-math` - Fast floating-point math

### Memory Management
- `USE_DYNAMIC_ARRAY` flag already present in codebase
- Reduces static memory allocation
- Important for 512MB RAM constraint

### Video Recording
- Disabled via CMake on Pocket CHIP builds
- CPU not powerful enough for real-time video encoding
- Screenshot functionality still available

## Build Process

### Dependencies Required
- Standard build tools (gcc, g++, make, cmake)
- OpenGL ES development libraries (libgles2-mesa-dev)
- EGL development libraries (libegl1-mesa-dev)
- GLFW3 with ES support (libglfw3-dev)
- ALSA audio (libasound2-dev)
- X11 libraries (for window management)

### Build Command
```bash
cmake -B build-pocketchip \
    -DCMAKE_BUILD_TYPE=Release \
    -DPOCKET_CHIP=ON \
    -DVIDEO_RECORDING=OFF
cmake --build build-pocketchip -j2
```

## Testing Recommendations

When testing on Pocket CHIP:

1. **Performance Testing**
   - Test all 150 levels for frame rate
   - Monitor memory usage
   - Check for thermal throttling

2. **Input Testing**
   - Verify all keyboard controls work
   - Test touchscreen input if applicable
   - Check menu navigation

3. **Graphics Testing**
   - Verify shader compilation
   - Check texture loading
   - Ensure proper viewport scaling

4. **Audio Testing**
   - Verify sound effects play correctly
   - Check music playback
   - Test ALSA output

## Known Limitations

1. **No video recording** - Disabled due to performance constraints
2. **Limited UI space** - 480x272 may make some ImGui dialogs cramped
3. **Gamepad support** - Untested on Pocket CHIP
4. **Performance** - Complex levels may drop below 60 FPS

## Future Improvements

Potential enhancements for the Pocket CHIP port:

1. **Touchscreen controls** - Optimize UI for touch input
2. **Performance profiling** - Identify bottlenecks for optimization
3. **Resolution scaling** - Option to render at lower res and upscale
4. **Power management** - CPU frequency scaling based on load
5. **Custom keyboard layout** - Remap controls for Pocket CHIP keyboard

## Compatibility

This port maintains backward compatibility:
- Desktop builds (Windows/Linux) unaffected
- Emscripten/WASM builds unaffected
- Android builds unaffected
- All changes are conditionally compiled

## License

Same as original project (check LICENSE.md)

## Credits

**Pocket CHIP Port**: Community contribution
**Original Project**: Ma-Pe-Ma (Margitai Péter Máté)
**Original Game**: Douglas E. Smith (1983)
