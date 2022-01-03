#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# custom project-wide macro, used to differentiate android and pc solutions
add_definitions(-DANDROID_VERSION)
add_definitions(-DRELEASE_VERSION)

cmake_minimum_required(VERSION 3.4.1)
project(LodeRunnerNativeActivity LANGUAGES C CXX)

# set up common compile options
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror -fno-exceptions -fno-rtti")

# create variable holding the path to the common dir
get_filename_component(commonDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../common ABSOLUTE)

# build the ndk-helper library
get_filename_component(ndkHelperSrc ${commonDir}/ndk_helper ABSOLUTE)
add_subdirectory(${ndkHelperSrc} ${commonDir}/ndkHelperBin/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

# build oboe sound api library
get_filename_component(oboeSrc ${commonDir}/oboe ABSOLUTE)
add_subdirectory(${oboeSrc} ${commonDir}/bin/oboe)

# build ogg library
get_filename_component(oggDir ${commonDir}/ogg ABSOLUTE)
add_subdirectory(${oggDir} ${commonDir}/bin/ogg)

# build vorbis library
get_filename_component(vorbisDir ${commonDir}/vorbis ABSOLUTE)
add_subdirectory(${vorbisDir} ${commonDir}/bin/vorbis)

# include the common (only android_fopen is used by this)
include_directories (${commonDir})

# Make the Oboe public headers available to our app
#include_directories (${oboeSrc}/include)

# include the game's headers
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Sources)

# now build app's shared lib
add_library(${PROJECT_NAME}
        SHARED
        Sources/GameStates/Begin.cpp
        Sources/GameStates/Death.cpp
        Sources/GameStates/GameState.cpp
        Sources/GameStates/Pause.cpp
        Sources/GameStates/Play.cpp
        Sources/States/GameOver.cpp
        Sources/States/GamePlay.cpp
        Sources/States/Generator.cpp
        Sources/States/Intro.cpp
        Sources/States/MainMenu.cpp
        Sources/States/Outro.cpp
        Sources/States/Select.cpp
        Sources/States/State.cpp
        Sources/States/StateContext.cpp
        Sources/Audio.cpp
        Sources/Brick.cpp
        Sources/Button.cpp
        Sources/Drawing.cpp
        Sources/Enemy.cpp
        Sources/GameTime.cpp
        Sources/GLHelper.cpp
        Sources/Gold.cpp
        Sources/IOHandler.cpp
        Sources/Player.cpp
        AudioCallback.cpp
        GameActivity.cpp
        Engine.cpp
        Helper.cpp
        LodeRunnerRenderer.cpp
        )
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set_target_properties(${PROJECT_NAME}
        PROPERTIES  LINK_FLAGS "-u ANativeActivity_onCreate")

# add lib dependencies
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        NdkHelper
        ogg
        vorbis
        vorbisenc
        vorbisfile
        oboe
        )