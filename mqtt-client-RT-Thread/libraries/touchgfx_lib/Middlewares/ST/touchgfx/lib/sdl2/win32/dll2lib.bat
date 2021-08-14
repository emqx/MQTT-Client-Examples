@echo off
rem The SDL2.DLL was downloaded from http://libsdl.org
rem The SDL2.LIB was created using "dumpbin" and "lib"
rem The libSDL2.a was created using "dumpbin" and "dlltool"

rem Visual Studio uses SDL2.lib for linking
rem Gcc uses libSDL2.a for linking
rem Both use SDL2.dll at runtime

set oldpath=%path%
set path="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin";C:\touchgfx-env\MinGW\msys\1.0\bin;C:\touchgfx-env\MinGW\bin;%path%

rem create SDL2.lib for Visual Studio
echo EXPORTS > SDL2.def
dumpbin /EXPORTS SDL2.dll | grep SDL_ | cut -c 27- >>SDL2.def
lib /def:SDL2.def /machine:x86 /out:SDL2.lib

rem create libSDL2.a for mingw32
echo LIBRARY SDL2.DLL >SDL2.def
echo EXPORTS >> SDL2.def
dumpbin /EXPORTS SDL2.dll | grep SDL_ | cut -c 27- >>SDL2.def
dlltool -d SDL2.def -l libSDL2.a

del SDL2.def
del SDL2.exp

rem create SDL2_image.lib for Visual Studio
echo EXPORTS > SDL2_image.def
dumpbin /EXPORTS SDL2_image.dll | grep IMG_ | cut -c 27- >>SDL2_image.def
lib /def:SDL2_image.def /machine:x86 /out:SDL2_image.lib

rem create libSDL2_image.a for mingw32
echo LIBRARY SDL2_image.DLL >SDL2_image.def
echo EXPORTS >> SDL2_image.def
dumpbin /EXPORTS SDL2_image.dll | grep IMG_ | cut -c 27- >>SDL2_image.def
dlltool -d SDL2_image.def -l libSDL2_image.a

del SDL2_image.def
del SDL2_image.exp


set path=%oldpath%
set oldpath=
