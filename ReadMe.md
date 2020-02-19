# Lode Runner 2020

This is a remake of the classic ['80s puzzle video game](https://en.wikipedia.org/wiki/Lode_Runner) (and its hardcore [sequel's](https://en.wikipedia.org/wiki/Championship_Lode_Runner) too)!
The original was developed by [Douglas E. Smith](https://en.wikipedia.org/wiki/Douglas_E._Smith) in 1983 for the [Apple 2](https://en.wikipedia.org/wiki/Apple_II) computer. This version is based on the [NES](https://en.wikipedia.org/wiki/Nintendo_Entertainment_System) port.

Before playing check out the manual of the [original](https://www.gamesdatabase.org/Media/SYSTEM/Nintendo_NES//Manual/formated/Lode_Runner_-_1987_-_Br%C3%B8derbund_Software.pdf) version. I could not find the manual  for the NES port of Championship Lode Runner but [here](https://mocagh.org/broderbund/masksun-loderunner-loderunner-manual.pdf) you can check out the Apple II's manual.

## Features of the game
### Hardver requirements
Your graphics card has to support OpenGL 4.5 to run it.
### Configuration file
In the root folder there is a simple txt configuration file in which you can change features of the game. Every entry has a description for it.

### Differences from the original (NES) version:
- controller input is supported but the mappings differ from the original 
- the whole level layout can be seen, no scrolling needed
- original Apple II levelset is used
- collision detection is not the same as in the original one I implemented a new one 
- guards' moving are calculated at every frame while in the original one they only moved at every nth (varying) frame
- if the runner falls through a trapdoor it will be marked
- enemies "wear" different suits when they carry gold
- level timer at top of the screen

 The levels are ripped from the original Apple II version. The NES port has only 50 levels and this remake includes the missing 100 levels too. Note: The levels ported to the NES are not 100% identical to the original ones, as some rows or columns are missing in the ported levels. If you have the proper NES levelset I will happily include it.

A conspicious difference about the collision detection: in the original when there are 3 guards circling around a ladder, they do not stop, while in this version they do, but this does not effect the gameplay much, I think...)

### Extra features:
- taking screenshot
- recording gameplay video

### Missing Features:
- in the NES version after collecting every lode a bonus fruit pickup appears randomly but I could not find the ripped textures so I could not include them
- Demo in main menu is not implented :(

### Summarizing what I did not create in this project:
- the game logic (naturally..)
- the textures (I found them on the web they were ripped from the nes cartridge)
- the soundeffects/music (some of them are form online sources but the rest were recorded by me with the help of an emulator)
- guard AI (the AI is the same as the original it was published in a book which I couldn't get but I found it in an other [remake](https://github.com/SimonHung/LodeRunner) and I translated it to C++)
- level design - same as original but copied from the mentioned remake

Every other feature was developed by me using existing libraries.


## Compilation help

Libraries needed to be linked to compile:
- [GLFW](https://www.glfw.org/) - utility used to create OpenGL contexts
- [RTAUDIO](https://www.music.mcgill.ca/~gary/rtaudio/) - used to play sound effects
- [libogg / libvorbis / libvorbisfile](https://xiph.org/downloads/) - used to decode vorbis sound files
- [FFMPEG](https://ffmpeg.org/) (avformat, avutil, avcodec, swscale, swresample) - needed for video recording
(Note: if VIDEO_RECORDING macro is not defined at the begining of main.cpp this one is not needed!)

Headers needed to be included:
- [STBI Image + Write](https://github.com/nothings/stb) - used to read textures and write screnshots
- [this shader class](https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h) - used to handle shaders easily

Source file needed to be included:
- [GLAD](https://glad.dav1d.de/) - an OpenGL Loading Library is a library that loads pointers to OpenGL functions at runtime, this is required to access functions from OpenGL

## Controls:
The game uses XInput API for controller handling.

	Keyboard    	Controller	    Effect

	arrows  	    left stick	    moving/change game mode at main menu
	q 			    LB/LT		    dig to the left
	w 			    RB/RT		    dig to the right
	enter 		    start		    pause, skip intro/outro
	space 		    back		    change level during gameplay
	
	p 			    -   		    take screenshot
	r 			    -			    start/end recording gameplay video
	
	alt + enter  	-   		    change window mode (full or windowed)
	esc  			-	    	    close application

## Download Links

Basic version
Video recording version

Just simply unpack the 7z archive and launch the Lode Runner.exe.

If you want to play with the Championship Lode Runner levels modify the proper line in the config.txt file or launch the app with "Championship" command line argument! (I suggest you to create a shortcut for it!)

Note: linux ports are currently not available.

## Developer Notes
I tested the game, I completed much of it but I realised there was a quite serious problem with the guard AI.
I fixed the problem, but it's possible that other bugs arised. Later I will play through it again and check if everything is OK or not.

This was my first complex project since I started programming. My earlier efforts were limited to calculate/solve math problems. This was a good starting project as I learned a lot about programming and designing code.

(Note: Midway through development I realized that some of my used techinques were quite bad so I rewrote many of it but some parts of it are still quite bad.)

## Future tasks for this project:
- rewrite code to follow a C++ style guide
- further FFMPEG optimization
- android version porting
- rewrite move interraction code as it is quite messy now
- adding some useful extras, eg. score counter, leaderboard, saving level completion time etc.
- create CMake geneator (note: currently Windows version is built but only a few lines needed to be rewritten to compile for linux)

However, in the near future I'm not planning to address this issues as I have other projects and I got bored by this a little. So it's possible that the next big update will only come in a few years from the initial commit.

## Have fun with playing it!