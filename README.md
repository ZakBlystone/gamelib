# Gamelib
Experimental game bootstrap library for the C Programming Language consisting of just a header and DLL

`Gamelib` contains minimal dependencies and is statically linked against the MSVC2015 runtime.

### Usage:

- Add `#define GAMELIB_WITH_BOOTSTRAP` to the top of one C file in your program (will generate code for DLL loading)
- Include `"lib.h"` in your C program
- Call `init_game_lib()` then `get_game_lib()` to get a pointer to the `gamelib_t` struct
- Set initialization parameters in a `initparams_t` struct.
- Call `init()` to create a window and start the game
- Loop and call `update()` (will return false when window has closed)
- Call `shutdown()`
- Cleanup with `free_game_lib()`

Sample code can be found in `src/test.c`

Works with most compilers on windows 32-bit / 64-bit.
