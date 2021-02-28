# Inga SDL

Inga is an engine for 2D point and click graphic adventures. Originally it was created for the never finished game Ermentrud on the Amiga. This is a platform independent re-implementation based on SDL2.

The Ermentrud game and the original Inga engine was created between 2001 and 2004. It required an Amiga with a graphics card and a hard disk. It's not easy to run it in an emulator and when you get it to work it has annoying glitches. I want to preserve my old projects, so I though about porting the engine to SDL2.

My first idea was to take the original C code and simply replace all Amiga functions with SDL2. But the code is really bad and it would be a lot of work to make it work. Just to still have bad code later, which wouldn't serve for anything else than to run my unfinished game. So I changed my mind.

The Inga SDL project is a complete reimplementation of all important features of the original engine, but much cleaner, more flexible and ready for future improvements.

## First Milestone

- New implementation of most old features
- Loads files in the formats of the old engine
- Interpreter based on code of the old engine
- Sound effects
- Music (ogg files instead of CD audio)
- 100% compatible with Ermentrud game
- No new authoring tools

## Future Ideas

- Support for new file formats (especially PNG)
- New script language
- New authoring tools

Currently the future ideas are halted, because I want to make something new in Unity now.

## Howto build with CMake

### Linux (Ubuntu)
- install required libraries:  
  sudo apt install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
- go to inga-sdl
- mkdir build && cd build
- cmake ../ -DCMAKE_BUILD_TYPE=Release
- cmake --build . --target all

### MacOsX
- install homebrew and CMake
- install required libraries:
  brew install sdl2 sdl2_gfx sdl2_image sdl2_mixer sdl2_ttf
- go to inga-sdl
- mkdir build && cd build
- cmake ../ -DCMAKE_BUILD_TYPE=Release
- cmake --build . --target all  
  
or your can generate a XCode project (example uses arm build):
- go to inga-sdl
- mkdir xc && cd xc
- cmake ../ -G Xcode -DCMAKE_OSX_ARCHITECTURES=arm64
- open the generated XCode project

## Howto build for MorphOs
- you will need SDL2 installed inclusive the SDL2 SDK
- SDL2 static libs should be in gg:usr/local/lib
- SDL2 header files should be in gg:/usr/local/include
- go to inga-sdl
- make -f Makefile.mos

## Howto build for AmigaOS 4
- you need to have SDL2 library installed, get it here:
- https://github.com/AmigaPorts/SDL/releases
- go to inga-sdl
- make -f Makefile.amigaos4

## Links

- https://www.inutilis.com/portfolio/inga-adventure-game-engine/
- https://www.inutilis.com/portfolio/ermentrud-das-spiel/
