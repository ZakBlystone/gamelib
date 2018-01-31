@echo off

if "%VCLIBBUILDTOOLS%" == "32" goto compile
call "G:/Visual Studio 2015/VC/vcvarsall" x86
set VCLIBBUILDTOOLS=32
echo environment: vcvarsall x86

:compile

cd bin32

del *.exe
del *.dll

cd ..

cl src/lib.c src/draw.c src/glad.c /Febin32/gamelib.dll /I "./include" /I "./thirdparty/include" /MT /link /DLL /LIBPATH:"./thirdparty/x32" glfw3.lib opengl32.lib kernel32.lib user32.lib shell32.lib gdi32.lib opengl32.lib
cl src/test.c /Febin32/game.exe /I "./include" /MT /link /SUBSYSTEM:CONSOLE

del *.obj

if exist "./bin32/game.exe" "./bin32/game.exe"