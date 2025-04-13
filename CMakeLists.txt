# Note: in VS you have to set the "currentDir" : "${workspaceRoot}" key-value inside the executable's debug configuration 
# Without this the dlls and the assets/resources are not found when running the app while developing

# to build emscripten version: emcmake cmake -B./out/build/wasm-release -G "Ninja" -DCMAKE_BUILD_TYPE=Release

cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("LodeRunner")

#set(BUILD_SHARED_LIBS TRUE)

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
	"src/loderunner/iocontext/rendering/Text.cpp"
	"src/loderunner/iocontext/rendering/RenderingManager.cpp"

	"src/submodules/imgui/backends/imgui_impl_opengl3.cpp"
	"src/submodules/imgui/backends/imgui_impl_glfw.cpp"
	"src/submodules/imgui/imgui.cpp"
	"src/submodules/imgui/imgui_draw.cpp"
	"src/submodules/imgui/imgui_tables.cpp"
	"src/submodules/imgui/imgui_widgets.cpp"
 	"src/loderunner/iocontext/GeneratorGUI.cpp"
)

# setting project for video recording (only Windows build was tested)
option(VIDEO_RECORDING "Build with video recording capabilites" OFF)

if(WIN32)
	# on windows generate icon file and add to the sources
	file(WRITE "assets/textures/icon.rc" "IDI_ICON1 ICON \"./RUNNER.ico\"")

	target_sources(LodeRunner PRIVATE
		"assets/textures/icon.rc"
	)

	# set up resources for developing
	file(MAKE_DIRECTORY "resources/screenshots")
	if(VIDEO_RECORDING)
		file(MAKE_DIRECTORY "resources/gameplay-videos")
	endif()
	
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/config.txt")
		file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/config.txt" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/resources")
	endif()	
endif()

# copy the font file
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/Roboto-Medium.ttf")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/imgui/misc/fonts/Roboto-Medium.ttf" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/assets")
endif()

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
	file(GLOB ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")

	set_target_properties(LodeRunner PROPERTIES
		SUFFIX ".html" # generate html
		LINK_FLAGS "--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets/" # link the assets folders
		LINK_DEPENDS "${ASSET_FILES}" # rebuild if an asset is changed
	)

	target_compile_options(LodeRunner PUBLIC 
		"-pthread;"
	)

	target_link_options(LodeRunner PUBLIC
		"-sUSE_GLFW=3;"
		"-sMAX_WEBGL_VERSION=2;-sMIN_WEBGL_VERSION=2;-lopenal"
		"-std=c++23;-fexperimental-library;"
		"-sPTHREAD_POOL_SIZE=4;-sALLOW_MEMORY_GROWTH;"
		"-sLLD_REPORT_UNDEFINED"
	)

	target_sources(LodeRunner PRIVATE
		"src/loderunner/iocontext/rendering/EmscriptenRenderingManager.cpp"
	)
else()
	add_subdirectory("src/submodules/glfw")
	set(RTAUDIO_TARGETNAME_UNINSTALL "rtuninstall" CACHE STRING "Name of 'uninstall' build target" FORCE)
	add_subdirectory("src/submodules/rtaudio")

	target_sources(LodeRunner PUBLIC		
		"src/submodules/glad.c"
	)

	target_link_libraries(LodeRunner PRIVATE
		glfw
		rtaudio
	)

	if(WIN32)
		target_compile_definitions(LodeRunner PRIVATE
			-DSOUND_API=RtAudio::WINDOWS_WASAPI
		)

		# Release mode
		if (CMAKE_BUILD_TYPE STREQUAL "Release")
			# Hide Console Window method 1
			#[[target_link_options(LodeRunner PUBLIC 
				"/SUBSYSTEM:WINDOWS;/ENTRY:mainCRTStartup;"
			)]]

			# Hide Console Window method 2
			set_target_properties(LodeRunner PROPERTIES 
				LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
			)
		# Debug mode
		else()
			# show console window
			set_target_properties(LodeRunner PROPERTIES 
				LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
			)
		endif()
	endif()

	if(UNIX)
		target_compile_definitions(LodeRunner PRIVATE
			-DSOUND_API=RtAudio::LINUX_ALSA
		)
	endif()
endif()

# Release mode
if (CMAKE_BUILD_TYPE STREQUAL "Release" OR EMSCRIPTEN)
	target_compile_definitions(LodeRunner PRIVATE
		-DRELEASE_VERSION
	)
endif()

# for developing this path needs to be added in the proper cmake preset: 
# "environment": { "PATH": "$penv{PATH};${sourceDir}/src/submodules/ffmpeg/bin/"}
if (VIDEO_RECORDING)
	# set macro used by game source
	target_compile_definitions(LodeRunner PRIVATE
		-DVIDEO_RECORDING
	)

	target_sources(LodeRunner PRIVATE
		"src/loderunner/multimediarecording/MultiMedia.cpp"
		"src/loderunner/multimediarecording/AudioStream.cpp"		
		"src/loderunner/multimediarecording/VideoStream.cpp"
	)

	if (WIN32)
		set(URL "https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full-shared.7z")
		set(submodulePath "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/")

		if(NOT EXISTS "${submodulePath}/ffmpeg/")
			file(DOWNLOAD ${URL} "${submodulePath}/ffmpeg.7z" SHOW_PROGRESS)
			file(ARCHIVE_EXTRACT INPUT "${submodulePath}/ffmpeg.7z" DESTINATION "${submodulePath}/ffmpeg/")
			file(REMOVE "${submodulePath}/ffmpeg.7z")
		endif()

		target_include_directories(LodeRunner PRIVATE 
			"${CMAKE_SOURCE_DIR}/src/submodules/ffmpeg/include"
		)

		target_link_directories(LodeRunner PRIVATE 
			"${CMAKE_SOURCE_DIR}/src/submodules/ffmpeg/lib"
		)
	endif()

	target_link_libraries(LodeRunner PRIVATE
		avformat
		avutil
		avcodec
		swscale
	)
endif()

# cmake --install /path/to/build --prefix /path/to/install --component loderunner
if(EMSCRIPTEN)
	set(em_target "LodeRunner")
	install(TARGETS LodeRunner COMPONENT loderunner RUNTIME DESTINATION ".")
	install(FILES
        "$<TARGET_FILE_DIR:${em_target}>/${em_target}.js"
        "$<TARGET_FILE_DIR:${em_target}>/${em_target}.wasm"
		"$<TARGET_FILE_DIR:${em_target}>/${em_target}.data"
		COMPONENT loderunner
        DESTINATION "."
	)
elseif(WIN32 OR UNIX)
	install(TARGETS LodeRunner COMPONENT loderunner RUNTIME DESTINATION ".")
	install(DIRECTORY "assets" COMPONENT loderunner DESTINATION "." PATTERN "*.rc" EXCLUDE PATTERN "config.txt" EXCLUDE)
	install(FILES "ReadMe.md" "LICENSE.md" COMPONENT loderunner DESTINATION ".")
	
	install(DIRECTORY COMPONENT loderunner DESTINATION "./resources/screenshots")
	install(FILES "assets/config.txt" COMPONENT loderunner DESTINATION "./resources/")
	
	if(VIDEO_RECORDING)
		install(DIRECTORY COMPONENT loderunner DESTINATION "./resources/gameplay-videos")

		if(WIN32)
			foreach(dll "avformat" "avutil" "avcodec" "swresample" "swscale")
				file(GLOB DllPath "src/submodules/ffmpeg/bin/${dll}-*.dll")
				install(FILES "${DllPath}" COMPONENT loderunner DESTINATION ".")
			endforeach()
		endif()
	endif()
endif()
