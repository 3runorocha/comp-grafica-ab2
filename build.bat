@echo off
rem Compila o projeto com GCC (MinGW-w64). Rode a partir da pasta do projeto.
setlocal

set SRC=src\main.c src\mathlib.c src\shader.c src\texture.c src\mesh.c src\camera.c src\background.c src\calib.c src\renderer.c src\node.c src\materials.c src\scene.c src\objects\desk.c src\objects\crate.c src\objects\stepladder.c src\objects\chair.c src\objects\chalkboard.c src\objects\globe.c src\objects\key.c src\objects\lamp.c src\objects\fire.c src\character.c src\player.c src\zones.c src\room.c src\sound.c src\shadow.c third_party\glad\src\gl.c
set INC=-isystem third_party\glfw\include -isystem third_party\glad\include -isystem third_party\stb -isystem third_party\miniaudio -Isrc
set LIBS=-Lthird_party\glfw\lib -lglfw3 -lopengl32 -lgdi32

gcc -std=c11 -O2 -Wall -Wextra %INC% %SRC% %LIBS% -o cg-ab2.exe
if errorlevel 1 (
    echo.
    echo *** Falha na compilacao ***
    exit /b 1
)
echo Compilado: cg-ab2.exe
