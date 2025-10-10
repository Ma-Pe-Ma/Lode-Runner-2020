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
	"src/loderunner/iocontext/rendering/Text.cpp"
	"src/loderunner/iocontext/rendering/RenderingManager.cpp"
	"src/loderunner/iocontext/GeneratorGUI.cpp"

	"src/submodules/imgui/backends/imgui_impl_opengl3.cpp"
	"src/submodules/imgui/backends/imgui_impl_glfw.cpp"
	"src/submodules/imgui/imgui.cpp"
	"src/submodules/imgui/imgui_draw.cpp"
	"src/submodules/imgui/imgui_tables.cpp"
	"src/submodules/imgui/imgui_widgets.cpp" 	
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
endif()

# copy resource files needed for installation
foreach(json "config")
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/${json}.json")
		file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/${json}.json" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/resources")
	endif()	
endforeach()	

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
	"src/submodules/json/single_include/nlohmann"
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
		"-fexceptions"
	)

	target_link_options(LodeRunner PUBLIC
		"-sUSE_GLFW=3;"
		"-sMAX_WEBGL_VERSION=2;-sMIN_WEBGL_VERSION=2;-lopenal"
		"-std=c++23;-fexperimental-library;"
		"-sPTHREAD_POOL_SIZE=4;-sALLOW_MEMORY_GROWTH;"
		"-sLLD_REPORT_UNDEFINED"
		"-sEXPORTED_FUNCTIONS=_main,_malloc,_free,_jsonFileLoaded"
		"-sEXPORTED_RUNTIME_METHODS=ccall,HEAPU8"
		"-fexceptions"
		"-sEXCEPTION_CATCHING_ALLOWED=[json.exception]"
	)

	target_sources(LodeRunner PRIVATE
		"src/loderunner/iocontext/EmscriptenIOContext.cpp"
		"src/loderunner/iocontext/rendering/EmscriptenRenderingManager.cpp"
	)

	if(NOT DEFINED LEVEL_URL)
		message(FATAL_ERROR "LEVEL_URL is not set. Please specify it with: -DLEVEL_URL=http://...")
	else()
		message("Levelset URL is set as: ${LEVEL_URL}")
	endif()

	target_compile_definitions(LodeRunner PRIVATE
		"LEVEL_URL=\"${LEVEL_URL}\""
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

	# Pocket CHIP specific configuration
	if(POCKET_CHIP)
		message(STATUS "Configuring for Pocket CHIP")

		# Use OpenGL ES 2.0 compatible mode (Mali-400 GPU)
		target_compile_definitions(LodeRunner PRIVATE
			-DPOCKET_CHIP
			-DIMGUI_IMPL_OPENGL_ES3
		)

		# Link against OpenGL ES libraries
		find_package(OpenGLES2 QUIET)
		if(OpenGLES2_FOUND)
			target_link_libraries(LodeRunner PRIVATE ${OPENGLES2_LIBRARIES})
			target_include_directories(LodeRunner PRIVATE ${OPENGLES2_INCLUDE_DIRS})
		else()
			# Fallback to manual linking if package not found
			target_link_libraries(LodeRunner PRIVATE GLESv2 EGL)
		endif()

		# Disable video recording on Pocket CHIP (limited resources)
		set(VIDEO_RECORDING OFF CACHE BOOL "Video recording disabled for Pocket CHIP" FORCE)

		message(STATUS "Pocket CHIP optimizations enabled")
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

		file(GLOB FFMPEG_DIR "${CMAKE_SOURCE_DIR}/src/submodules/ffmpeg/*/")

		target_include_directories(LodeRunner PRIVATE
			"${FFMPEG_DIR}/include"
		)

		target_link_directories(LodeRunner PRIVATE 
			"${FFMPEG_DIR}/lib"
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
	install(DIRECTORY "assets" COMPONENT loderunner DESTINATION "." PATTERN "*.rc" EXCLUDE PATTERN "config.json" EXCLUDE)
	install(FILES "ReadMe.md" "LICENSE.md" COMPONENT loderunner DESTINATION ".")
	
	install(DIRECTORY COMPONENT loderunner DESTINATION "./resources/screenshots")
	install(FILES "assets/config.json" COMPONENT loderunner DESTINATION "./resources/")
	
	if(VIDEO_RECORDING)
		install(DIRECTORY COMPONENT loderunner DESTINATION "./resources/gameplay-videos")

		if(WIN32)
			foreach(dll "avformat" "avutil" "avcodec" "swresample" "swscale")
				file(GLOB DllPath "src/submodules/ffmpeg/*/bin/${dll}-*.dll")
				install(FILES ${DllPath} COMPONENT loderunner DESTINATION ".")
			endforeach()
		endif()
	endif()
endif()
