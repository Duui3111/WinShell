#ifndef __SHELL_H__
#define __SHELL_H__
#include "includes/colors.h"

#define ERRORF(x) printf(x);

const char invarg[] = "invalid arguments\n";
const char needarg[] = "missing arguments\n";
const char nofile[] = "no such file or directory\n";
const char nocommand[] = "command not found\n";
const char cwderror[] =  "couldn't get current directory\n";

typedef struct s_string_array {
    int size;
    char** array;
} string_array;

void getdir(char dirbuffer[]);
char* join(string_array* param_1, char* param_2);
void cd(int* argcount, char* argumente[], char cwdbuffer[BUFSIZ]);
void date(int* argcount);
void runenv(int *argcount, char** argumente);
void setTextColor(FILE *stream, TextColor color);
char* string_append(char * buffer, size_t buffer_size, const char * string);
void getenvAll(int *argcount, char *argumente[]);
static void list_dir(int *argcount, char *argumente[]);
void cls(int *argcount, char *argumente[]);
void copy(int *argcount, char *argumente[]);
void math(int *argcount, char *argumente[]);
void cwd(int *argcount, char *argumente[], char dirbuffer[BUFSIZ], char cwdbuffer[BUFSIZ]); 
void WinShell();

#endif
