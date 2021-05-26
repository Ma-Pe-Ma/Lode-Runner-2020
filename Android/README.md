# Lode Runner 2020 Android

This is the Android port of Lode Runner 2020 which was also made by me. For none Android specific information please check out the orginal repo's ReadMe.
This version was based on the more teapots native activity example, many tools are taken from there.

## Building

1. Clone the repository with Android Studio.
2. Then clone oboe, ogg, vorbis libraries to the /common folder.
3. Replace the proper vorbis cmakelists.txt files with the provided ones.

For the second entry please check out the project's CMakeLists.txt (LodeRunnerAndroid/src/main/cpp/CMakeLists.txt) for naming the libs' folder names correctly.
In the future maybe I will add the mentioned libraries as submodules to the project to easen the building process (ie. elminiating the 2. entry).

## Known bugs

1. Pausing the game is messy when switching to other apps (not implemented yet). Switching between activities are messy too!!
2. Texture mapping is misplaced somehow
3. Blending is messed up too!

## Future plans

1. Implement controlling by motion sensors.
2. Fix mentioned bugs.

**Have fun playing with it!**