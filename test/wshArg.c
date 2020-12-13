// gcc wshArg.c

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>

int main(int argc, char * argv[])
{
    HMODULE libHandle;

    if ((libHandle = LoadLibrary(TEXT("../build/WinShell.dll"))) == NULL)
    {
        printf("load failed\n");
        return 1;
    }
    void (*date)(int* argcount) = GetProcAddress(libHandle, "date");
    date(&argc);
    system("pause");
    return 0;
}