# Inga SDL

Inga is an engine for 2D point and click graphic adventures. Originally it was created for the never finished game Ermentrud on the Amiga. This is a platform independent re-implementation based on SDL2.

The Ermentrud game and the original Inga engine was created between 2001 and 2004. It required an Amiga with a graphics card and a hard disk. It's not easy to run it in an emulator and when you get it to work it has annoying glitches. I want to preserve my old projects, so I though about porting the engine to SDL2.

My first idea was to take the original C code and simply replace all Amiga functions with SDL2. But the code is really bad and it would be a lot of work to make it work. Just to still have bad code later, which wouldn't serve for anything else than to run my unfinished game. So I changed my mind.

The Inga SDL project is a complete reimplementation of all important features of the original engine, but much cleaner, more flexible and ready for future improvements.

## First Milestone

- New implementation of most old features
- Loads files in the formats of the old engine
- Interpreter based on code of the old engine
- 100% compatible with Ermentrud game
- No sound
- No new authoring tools

## Future Ideas

- Support for new file formats (especially PNG)
- Sound effects and music
- New script language
- New authoring tools

## Links

https://www.inutilis.com/portfolio/inga-adventure-game-engine/
https://www.inutilis.com/portfolio/ermentrud-das-spiel/
