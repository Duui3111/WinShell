#include <iostream>
#include <fstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>  
#include <io.h>

using namespace std;

string WIN_mkdir = "mkdir";
string WIN_echo = "echo";


int main(int argc, char *argv[]) {   
    ifstream myfile(argv[1]);
    string line;
    string line2;
    string line3;
    getline (myfile, line3);
    getline (myfile, line);
    getline (myfile, line2);
    if(line.find(WIN_mkdir) && getline(myfile, WIN_mkdir)) {     
        ofstream(line.substr(6));
    } 
    if(line.find(WIN_echo) &&  getline (myfile, WIN_echo))  {
        cout << line2.substr(5) << endl;
    }
    if(!line.find(WIN_echo) || !line.find(WIN_mkdir))  {
        system(line2.c_str());
    }
    system("pause");
    return 0;  
}