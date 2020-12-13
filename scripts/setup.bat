@echo off 
:build
    cd ../
    mkdir bin 
    g++ winshell/winshell.cpp -o bin/wsh_lang.exe
    gcc shell.c -o bin/winshell.exe
    g++ plugins/calc.cpp -o bin/wsh_calc.exe
    mkdir build
    windres res.rc build/res.o
    gcc shell.c build/res.o -o bin/winshell.exe
    gcc -c -DBUILD_DLL shell.c -o build/WinShell.o
    gcc -shared -o build/WinShell.dll build/WinShell.o
    cd app
    msbuild.exe /p:Configuration=Release
    cd ../
    if exist ("%cd%\bin") {
        set PATH = "%cd%\bin; %PATH%"
    } else goto :build
echo done.
pause