@echo off

if "%VCLIBBUILDTOOLS%" == "64" goto compile
call "G:/Visual Studio 2015/VC/vcvarsall" x64
set VCLIBBUILDTOOLS=64
echo environment: vcvarsall x64

:compile

cd bin64

del *.exe
del *.dll

cd ..

cl src/lib.c src/draw.c src/glad.c /Febin64/gamelib64.dll /I "./include" /I "./thirdparty/include" /MT /link /DLL /LIBPATH:"./thirdparty/x64" glfw3.lib opengl32.lib kernel32.lib user32.lib shell32.lib gdi32.lib opengl32.lib
cl src/test.c /Febin64/game.exe /I "./include" /MT /link /SUBSYSTEM:CONSOLE

del *.obj

if exist "./bin64/game.exe" "./bin64/game.exe"