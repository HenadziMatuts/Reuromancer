# Reuromancer - Neuromancer Windows Port

Reuromancer is a 64-bit Windows port of MS-DOS version of the [**Neuromancer**][1] game (Interplay Productions, 1988) based on the William Gibson's novel of the same name.

<img src="https://habrastorage.org/webt/fe/9d/o8/fe9do8rmm54duesh6-k4f6wkcrm.gif" width="100%" height="100%"/>

## Project Structure

Basically there are three projects within one VS 2015 solution.

* _LibNeuroRoutines_ - a library that contains reverse engineered Neuromancer general purpose algorithms;
* _NeuromancerWin64 (startup project)_ - an engine itself. Currently it consumes [_CSFML_][2] (_SFML_ bindings for _C_ language) for multimedia things;
* _ResourceBrowser_ - an _MFC_ utility that allows to observe and export game resources from distribution _.DAT_-files.

_LibNeuroRoutines_ is a standalone project. The other two are both dependent on _LibNeuroRoutines_ and _CSFML_.

## Portability (why Win64 only?)

The only thing preventing me from make this portable is that _LibNeuroRoutines_ contains some 64-bit _MASM_ (_Microsoft Macro Assembler_) code. This _MASM_ is nothing but chunks of original disassembled code tweaked to be compilable and runable on the 64-bit machine. No doubt that there are portable _NASM_/_FASM_/... but I need that code to be debuggable in my _IDE_ and since it is _MSVS 2015_ - _MASM_ is the winner.

In future it is planned to replace all _MASM_ with the similar _C_-code. Following that there will be no problems running it on different platforms (except _ResourceBrowser_ as this is the _MFC_-application).

## How to Build

1. Get the sources (from this repo), extract it anywhere;
2. Get [_CSFML build_][2] (Visual C++ / GCC - 64-bit), extract it to the directory named _"External"_ (create it) that is inside the _Reuromancer_ root folder;
3. Open _Reuromancer.sln_ in your _Visual Studio_ (I use 2015, it also should work on 2013 and 2017, can't say about the others);
4. Set build configuration to x64 Debug and build it!

## How to Run

1. Build it (see above);
2. Get _Neuromancer_ MS-DOS distribution and copy _.DAT_-files to the directory where executables are located (__./x64/Debug/__) or to the project-specific subfolders if you run it from _IDE_;
3. Copy _CSFML_ binaries from _"External"_ folder (__./External/CSFML/bin/__) to the directory where executables are located (__./x64/Debug/__);
4. Get [_OpenAL Soft_][3] (Win32 and Win64 binaries), extract it anywhere. Copy __./openal-soft-1.18.2-bin/bin/Win64/soft_oal.dll__ to the directory where executables are located (__./x64/Debug/__) and rename it to the __openal32.dll__ (CSFML Audio dependency);
5. Run it!

## TODO

_ResourceBrowser_:
* rewrite audio player using _WinApi_ only to remove _CSFML_ dependency. 

## Development blog (RU):

* [Part 1][4]
* [Part 2][5]
* [Part 3][6]
* [Part 4][7]

[1]:https://en.wikipedia.org/wiki/Neuromancer_(video_game)
[2]:https://www.sfml-dev.org/download/csfml/
[3]:http://kcat.strangesoft.net/openal.html#download
[4]:https://habr.com/post/352050/
[5]:https://habr.com/post/357972/
[6]:https://habr.com/post/415555/
[7]:https://habr.com/post/417639/
