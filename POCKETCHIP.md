# Lode Runner 2020 - Pocket CHIP Port

This document contains instructions for building and running Lode Runner 2020 on the Pocket CHIP handheld device.

## Hardware Requirements

- **Pocket CHIP** handheld computer
- **CPU**: 1GHz ARM Cortex-A8 (Allwinner R8)
- **GPU**: Mali-400 (OpenGL ES 2.0/3.0)
- **RAM**: 512MB
- **Display**: 480x272 pixels (4.3" touchscreen)

## Features of the Pocket CHIP Port

- **OpenGL ES 3.0 support** - Optimized for Mali-400 GPU
- **Native 480x272 resolution** - Perfect fit for Pocket CHIP's screen
- **ARM-optimized build** - Uses NEON and hardware floating point
- **Low memory footprint** - Optimized for 512MB RAM
- **Keyboard controls** - Uses Pocket CHIP's built-in keyboard
- **Touchscreen support** - Via GLFW touch input (experimental)

## Building on Pocket CHIP

### 1. Clone the Repository

On your Pocket CHIP, clone the repository with submodules:

```bash
git clone --recurse-submodules -j2 https://github.com/YOUR-USERNAME/Lode-Runner-2020.git
cd Lode-Runner-2020
```

### 2. Run the Build Script

The build script will install dependencies and compile the game:

```bash
chmod +x build-pocketchip.sh
./build-pocketchip.sh
```

This will:
- Install required development packages
- Configure CMake with Pocket CHIP optimizations
- Build the game (takes ~10-15 minutes on Pocket CHIP)

### 3. Manual Build (Alternative)

If you prefer to build manually:

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake git libasound2-dev \
    libgles2-mesa-dev libegl1-mesa-dev libglfw3-dev \
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev \
    libxi-dev libxxf86vm-dev

# Configure and build
mkdir -p build-pocketchip
cmake -B build-pocketchip \
    -DCMAKE_BUILD_TYPE=Release \
    -DPOCKET_CHIP=ON \
    -DVIDEO_RECORDING=OFF

cmake --build build-pocketchip -j2
```

## Running the Game

### From Build Directory

```bash
cd build-pocketchip
cp ../assets/config-pocketchip.json ../resources/config.json
./LodeRunner
```

### Install System-Wide (Optional)

```bash
sudo cmake --install build-pocketchip --prefix /usr/local/games/loderunner
cd /usr/local/games/loderunner
cp assets/config-pocketchip.json resources/config.json
./LodeRunner
```

## Controls

The game is optimized for Pocket CHIP's keyboard:

| Key(s)          | Action                                    |
|-----------------|-------------------------------------------|
| Arrow Keys      | Move player (left, right, up, down)       |
| Q               | Dig left                                  |
| W               | Dig right                                 |
| Enter           | Pause / Skip intro/outro                  |
| Space           | Change level (during gameplay)            |
| Esc             | Pause / Exit                              |
| P               | Take screenshot                           |
| C               | Show configuration dialog                 |

**Note**: Video recording (R key) is disabled on Pocket CHIP due to limited resources.

## Performance Tips

### Optimizing Frame Rate

If you experience slowdowns, you can adjust settings in `resources/config.json`:

```json
{
  "FPS": 30,  // Reduce from 60 to 30 for better performance
  "playerSpeed": 0.9,
  "enemySpeed": 0.415
}
```

### Memory Considerations

- The game uses dynamic arrays (`USE_DYNAMIC_ARRAY` flag) to reduce memory usage
- Close other applications before running for best performance
- Avoid running in the background with many other processes

## Display Configuration

The Pocket CHIP port is configured for the native 480x272 resolution. The game will run fullscreen by default.

To adjust the viewport, edit `resources/config.json`:

```json
{
  "resolutions": [
    {
      "x": 480,
      "y": 272
    }
  ],
  "resolutionID": 0
}
```

## Troubleshooting

### OpenGL ES Errors

If you see OpenGL-related errors:

```bash
# Verify OpenGL ES support
glxinfo | grep "OpenGL ES"

# Should show: OpenGL ES profile version string: OpenGL ES 3.0
```

### GLFW/Window Creation Fails

```bash
# Ensure X11 is running
echo $DISPLAY  # Should output :0 or similar

# If blank, start X11
startx
```

### Build Fails - Out of Memory

The Pocket CHIP has limited RAM. If compilation fails:

```bash
# Build with only 1 core (slower but uses less memory)
cmake --build build-pocketchip -j1
```

### Missing Dependencies

If submodules are missing:

```bash
git submodule update --init --recursive
```

## Technical Details

### Compiler Flags Used

```
-march=armv7-a          # Target ARM v7 architecture
-mtune=cortex-a8        # Optimize for Cortex-A8 CPU
-mfpu=neon              # Use NEON SIMD instructions
-mfloat-abi=hard        # Hardware floating point
-O2                     # Optimization level 2
-ffast-math             # Fast math operations
```

### OpenGL ES Compatibility

The game automatically detects Pocket CHIP and uses:
- OpenGL ES 3.0 context
- GLSL ES 300 shaders
- `#version 300 es` shader version
- `precision highp float` for precision

### Libraries Used

- **GLFW** - Window management and input (OpenGL ES mode)
- **OpenGL ES 3.0** - Graphics rendering via Mali-400
- **RtAudio** - Audio playback (ALSA backend)
- **libogg/libvorbis** - Sound file decoding
- **ImGui** - Configuration UI

## Known Limitations

1. **Video recording disabled** - Not enough CPU/RAM for real-time encoding
2. **Limited ImGui support** - Some UI elements may be cramped at 480x272
3. **Gamepad support** - Limited/untested on Pocket CHIP
4. **Performance** - Some levels with many enemies may experience frame drops

## Getting the Best Experience

1. **Close background apps** to free up RAM
2. **Use wired audio** if possible for lower latency
3. **Reduce FPS to 30** if experiencing slowdowns
4. **Play Championship mode** for the hardcore challenge!

## Contributing

If you make improvements to the Pocket CHIP port, please submit a pull request!

## Credits

- **Original Game**: Douglas E. Smith (1983)
- **This Remake**: Margitai Péter Máté (Ma-Pe-Ma)
- **Pocket CHIP Port**: Community contribution

---

**Enjoy playing Lode Runner on your Pocket CHIP!** 🎮
