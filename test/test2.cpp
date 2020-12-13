#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>

typedef void (*EntryPointfuncPtr)();  

int main(int argc, char * argv[]) {
    HMODULE libHandle;

    if ((libHandle = LoadLibrary(TEXT("../build/WinShell.dll"))) == NULL)
    {
        std::cout << "load failed" << std::endl;
        return 1;
    }

    EntryPointfuncPtr LibMainEntryPoint;            
    LibMainEntryPoint = (EntryPointfuncPtr)GetProcAddress(libHandle, "WinShell");
    system("pause");
    return 0;
}