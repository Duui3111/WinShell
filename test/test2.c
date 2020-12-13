#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

int main(int argc, char * argv[])
{
    HMODULE libHandle;

    if ((libHandle = LoadLibrary(TEXT("../build/WinShell.dll"))) == NULL)
    {
        printf("load failed\n");
        return 1;
    }
    void (*start)(void) = GetProcAddress(libHandle, "WinShell");
    start();
    system("pause");
    return 0;
}
