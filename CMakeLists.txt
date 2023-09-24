# Note: in VS you have to set the "currentDir" : "${workspaceRoot}" key-value inside the executable's debug configuration 
# Without this the dlls and the assets/resources are not found when running the app while developing

# to build emscripten version: emcmake cmake -B./out/build/wasm -G "Ninja" -DCMAKE_BUILD_TYPE=Release

cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project("LodeRunner")

#set(BUILD_SHARED_LIBS TRUE)
message("COMPILER: ${CMAKE_CXX_COMPILER}")

# define executable and add src/loderunner source files
add_executable(LodeRunner
	"src/loderunner/Main.cpp"
	"src/loderunner/AppContainer.cpp"
	
	"src/loderunner/gameplay/Brick.cpp"
	"src/loderunner/gameplay/Enemy.cpp"
	"src/loderunner/gameplay/GameContext.cpp"
	"src/loderunner/gameplay/Player.cpp"

	"src/loderunner/states/GameOver.cpp"
	"src/loderunner/states/GamePlay.cpp"
	"src/loderunner/states/Generator.cpp"
	"src/loderunner/states/Intro.cpp"
	"src/loderunner/states/MainMenu.cpp"
	"src/loderunner/states/Outro.cpp"
	"src/loderunner/states/Select.cpp"
	"src/loderunner/states/State.cpp"
	"src/loderunner/states/StateContext.cpp"

	"src/loderunner/states/gamestates/Begin.cpp"
	"src/loderunner/states/gamestates/Death.cpp"
	"src/loderunner/states/gamestates/GameState.cpp"
	"src/loderunner/states/gamestates/Pause.cpp"
	"src/loderunner/states/gamestates/Play.cpp"

	"src/loderunner/iocontext/Button.cpp"
	"src/loderunner/iocontext/GlfwIOContext.cpp"
	"src/loderunner/iocontext/EmscriptenIOContext.cpp"
	"src/loderunner/iocontext/audio/Audio.cpp"
	"src/loderunner/iocontext/rendering/Text.cpp"
	"src/loderunner/iocontext/rendering/RenderingManager.cpp"

	"src/submodules/imgui/backends/imgui_impl_opengl3.cpp"
	"src/submodules/imgui/backends/imgui_impl_glfw.cpp"
	"src/submodules/imgui/imgui.cpp"
	"src/submodules/imgui/imgui_draw.cpp"
	"src/submodules/imgui/imgui_tables.cpp"
	"src/submodules/imgui/imgui_widgets.cpp"
)

# build external libraries
add_subdirectory("${CMAKE_SOURCE_DIR}/src/submodules/ogg")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/submodules/vorbis")

# Add include search paths for the application
target_include_directories(LodeRunner PRIVATE
	"src/loderunner"
	"src/submodules/"
	"src/submodules/stb/"
	"src/submodules/rtaudio/"
	"src/submodules/ogg/include/"
	"src/submodules/vorbis/include/"
	"src/submodules/imgui/"
	"src/submodules/glfw/include"
)

# link libraries with the app
target_link_libraries(LodeRunner PRIVATE
	ogg
	vorbis
	vorbisenc
	vorbisfile
)

if(EMSCRIPTEN)
	# deprecated
	#[[string(ASCII 27 Esc)
	set(ColourReset "${Esc}[m")
	set(ColourBold "${Esc}[1m")
	set(BoldRed "${Esc}[1;31m")
	set(Red "${Esc}[31m")
	#message(WARNING "${BoldRed} WARNING: Do not forget to specify the location of the page for the cookies!${ColourReset}")]]

	# generate html file
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	
	# configure opengl context + pthreads
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sLLD_REPORT_UNDEFINED \
		-s USE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 \
		-pthread -s USE_PTHREADS=1 -sPTHREAD_POOL_SIZE=4 -sALLOW_MEMORY_GROWTH \
		-lopenal 
		"
	)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fexperimental-library")

	set_target_properties(LodeRunner
		PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_SOURCE_DIR}/Assets@/Assets/ "
	)

	target_compile_options(LodeRunner
		PUBLIC "-pthread;-sUSE_PTHREADS=1;-sPTHREAD_POOL_SIZE=4;-sALLOW_MEMORY_GROWTH=1;"	
	)

	target_sources(LodeRunner PRIVATE
		"src/loderunner/iocontext/rendering/EmscriptenRenderingManager.cpp"
	)

else()
	add_subdirectory("src/submodules/glfw")
	add_subdirectory("src/submodules/rtaudio")

	target_sources(LodeRunner PUBLIC		
		"src/submodules/glad.c"
	)

	target_link_libraries(LodeRunner PRIVATE
		glfw
		rtaudio
	)
endif()

# setting project for video recording
# currently not working as ffmpeg update broke some functionalites (some members became const)
# earlier only Windows build was tested
option(VIDEO_RECORDING "Build with video recording capabilites" OFF)

# download libraries from https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full-shared.7z and unpack it to ./src/submodules/ffmpeg
if (VIDEO_RECORDING AND WIN32)
	# set macro used by game source 
	add_definitions(-DVIDEO_RECORDING)

	target_sources(LodeRunner PRIVATE
		"src/loderunner/MultiMediaRecording/AudioParameters.cpp"
		"src/loderunner/MultiMediaRecording/AudioStream.cpp"
		"src/loderunner/MultiMediaRecording/MultiMedia.cpp" 
		"src/loderunner/MultiMediaRecording/MultiMediaHelper.cpp"
		"src/loderunner/MultiMediaRecording/Stream.cpp"
		"src/loderunner/MultiMediaRecording/VideoParameters.cpp"
		"src/loderunner/MultiMediaRecording/VideoStream.cpp"
	)

	target_include_directories(LodeRunner PRIVATE "${CMAKE_SOURCE_DIR}/src/submodules/ffmpeg/include")

	target_link_directories(LodeRunner PRIVATE "${CMAKE_SOURCE_DIR}/src/submodules/ffmpeg/lib")

	target_link_libraries(LodeRunner PRIVATE
		avformat
		avutil
		avcodec
		swscale
		swresample
	)

	list(APPEND copyable_dlls
		"swscale-6"
		"swresample-4"
		"avutil-57"
		"avformat-59"
		"avcodec-59"
	)

	set(dll_extension "dll")

	# copy the dlls to working directory
	foreach(copyable_dll IN LISTS copyable_dlls)
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${copyable_dll}.${dll_extension}")
			message(STATUS "Copying ${copyable_dll} ...")
			file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/ffmpeg/bin/${copyable_dll}.${dll_extension}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}")
		else()
			message(STATUS "DLL ${copyable_dll} already copied.")
		endif()
	endforeach()
endif()

# Release mode
if (CMAKE_BUILD_TYPE STREQUAL "Release" OR EMSCRIPTEN)
	add_definitions(-DRELEASE_VERSION)
endif()

# windows specific settings
if (WIN32)
	add_definitions(-DSOUND_API=RtAudio::WINDOWS_WASAPI)

	# Release mode
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		# Hide Console Window method 1 - not working as cmake appends /SUBSYSTEM:CONSOLE to CMAKE_EXE_LINKER_FLAGS after running current CMakeLists.txt
#		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

		# Hide Console Window method 2 - no effect, the cause is probably is the same as in method 1
#		target_link_options(DrinkCoolingVisualizer
#			PUBLIC "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
#		)

		# Hide Console Window method 3 - working
		set_target_properties(LodeRunner
			PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
		)

		# runtime libraries, earlier they were necessary
		#target_link_libraries(DrinkCoolingVisualizer PRIVATE
			#msvcmrt	
			#msvcrt			
		#)

	# Debug mode
	else()
		# show console window
		set_target_properties(LodeRunner
			PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
		)
	endif()
elseif(UNIX)
	add_definitions(-DSOUND_API=RtAudio::LINUX_ALSA)
endif()