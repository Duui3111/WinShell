#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <conio.h>
#include <windows.h>  
#include <io.h>
#include <tchar.h>

#include "shell.h"

void getdir(char dirbuffer[]) {
	if(getcwd(dirbuffer, BUFSIZ) == NULL) {
		exit(1);
	}
}

void cd(int* argcount, char* argumente[], char cwdbuffer[BUFSIZ]) {
    if(*argcount != 1) {
		ERRORF(needarg);
        return;
    }

    int directory = chdir(argumente[1]);
    if(directory == -1) {
	   
    } else getdir(cwdbuffer-2);
}

void date(int* argcount) {
    if(*argcount > 0) {
		printf("you can only get the current date for now\n");
    } else {
   		int hours, minutes, seconds, day, month, year;
		time_t now;
		time(&now);
	
		struct tm *local = localtime(&now);
	
		hours = local->tm_hour;      
		minutes = local->tm_min; 
		seconds = local->tm_sec;     
	
		day = local->tm_mday;        
		month = local->tm_mon + 1;
		year = local->tm_year + 1900; 
	
		if (hours < 12)
        	printf("Time is : %02d:%02d:%02d am\n", hours, minutes, seconds);
 
    	else   
        	printf("Time is : %02d:%02d:%02d pm\n", hours - 12, minutes, seconds);
 
    	printf("Date is : %02d/%02d/%d\n", day, month, year);	
    }
}

char * join(string_array* param_1, char* param_2) {
    int total_len = 0;

    for(int i = 0; i < param_1->size; i++) {
        total_len += strlen(param_1->array[i]) + strlen(param_2);
    }
    total_len += 1;   
    char* new_word = malloc(total_len);
    if (new_word == NULL || param_1->array == NULL) {
        return NULL;
    }

    new_word[0] = '\0';  

    for (int i = 0; i < param_1->size; i++) {
        if (i > 0) {
           strcat(new_word, param_2);
        }
        strcat(new_word, param_1->array[i]);
    }
    return new_word;
}


void runenv(int *argcount, char** argumente) {
    string_array array_1;
    char* separator = " ";
    array_1.size = *argcount;
    array_1.array = &argumente[1];
    join(&array_1, separator);
    system(join(&array_1, separator));
}  


void echo(int *argcount, char** argumente) {
	char* argbuf;
	for(int i=1; i < *argcount+1; i++) {
		argbuf = malloc(strlen(argumente[i]) +1);
		if(argbuf != NULL) {
			strcpy(argbuf, argumente[i]);
			strcat(argbuf, " ");
			printf("%s", argbuf);
		}
	}
	printf("\n");
} 

char* string_append(char * buffer, size_t buffer_size, const char * string) {
	size_t buffer_len = strlen(buffer);
	size_t free_size = buffer_size - buffer_len;
	size_t string_len = strlen(string);
	if(free_size > string_len) {
		strncpy(buffer + buffer_len, string, string_len);
	}
}

void getenvAll(int *argcount, char *argumente[]) {
	printf("%s", getenv("PATH"));
}

static void list_dir(int *argcount, char *argumente[]) {
  	DIR *dir;
    struct dirent *directory;     
    dir = opendir ("./");
    char* dirout;

    if (argumente[1] != NULL) {
	  dir = opendir(argumente[1]);
    }

    if (dir != NULL) {
      while (directory = readdir (dir)) {
        dirout = directory->d_name;
        if (strcmp(dirout,".")!=0 && strcmp(dirout,"..") !=0) {
          puts(dirout); 
        }
      }
      (void)closedir(dir);
  	} else {
    ERRORF(nofile);
  }
}

void setTextColor(FILE *stream, TextColor color) {
    int outfd = fileno(stream);
    HANDLE out = (HANDLE)_get_osfhandle(outfd);
    DWORD outType = GetFileType(out);
    DWORD mode;
    if (outType == FILE_TYPE_CHAR && GetConsoleMode(out, &mode)) {
        SetConsoleTextAttribute(out, color);
    }
}

void cls(int *argcount, char *argumente[]) {
	if(argumente[1] != NULL) {  
     // ERRORF(invarg);
    }

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD coordScreen = { 0, 0 };  
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; 
    DWORD dwConSize;

   if(!GetConsoleScreenBufferInfo(hStdout, &csbi));

   dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

   if(!FillConsoleOutputCharacter(hStdout, (TCHAR) ' ', dwConSize, coordScreen,&cCharsWritten));
   if(!GetConsoleScreenBufferInfo(hStdout, &csbi));
   if(!FillConsoleOutputAttribute(hStdout, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten));

   SetConsoleCursorPosition(hStdout, coordScreen);
}

void copy(int *argcount, char *argumente[]) {
   char ch;
   FILE *source, *target;
 
   source = fopen(argumente[1], "r");
   if(source == NULL) {
		ERRORF(needarg);
		return;
   } 
   target = fopen(argumente[2], "w");
 
   if(target == NULL) {
      fclose(source);
   	}
 
   while((ch = fgetc(source)) != EOF) fputc(ch, target);
   printf("File copied successfully.\n");
 
   fclose(source);
   fclose(target);
}	

void cwd(int *argcount, char *argumente[], char dirbuffer[BUFSIZ], char cwdbuffer[BUFSIZ]) {
	if(cwdbuffer != NULL) {
		getdir(dirbuffer-2);
		strcpy(cwdbuffer, dirbuffer);
		printf("current working directory is: %s\n", cwdbuffer-2);
	} else {
		ERRORF(cwderror);
	}
}

void math(int *argcount, char *argumente[]) {
	char string_all[BUFSIZ];
	memset(string_all, 0x00, sizeof(string_all));

	char string[64];
	char* bytes_copied = 0;

  	snprintf(string, sizeof(string), "wsh_calc.exe %s", argumente[1]);
	bytes_copied = string_append(string_all, sizeof(string_all), string);

  	system(string_all);
}

void WinShell() {
	int argcount;
	char inbuffer[BUFSIZ], dirbuffer[BUFSIZ], workingbuffer[BUFSIZ], cwdbuffer[BUFSIZ];
	char **argumente = NULL;

	getdir(dirbuffer);
	strcpy(cwdbuffer, dirbuffer);

	char dest[2]={ 0 };
	memcpy(dest, cwdbuffer,sizeof(cwdbuffer));

	SetConsoleTitle("Winshell");


	while (1) {		
		setTextColor(stdout, TC_GREEN);
		printf("\n%s@%s ", getenv("USERNAME"), getenv("COMPUTERNAME"));
		setTextColor(stdout, TC_LIGHTMAGENTA);
		printf("~%s ", cwdbuffer);
		setTextColor(stdout, TC_BLUE);
		printf("$ ");
		setTextColor(stdout, TC_LIGHTGRAY);
		fgets(inbuffer, BUFSIZ, stdin);

		do {
			argumente = malloc(sizeof(char *) * 2);
			argumente[argcount = 0] = strtok(inbuffer, "\n\t ");
			while ((argumente[argcount + 1] = strtok(NULL, "\n\t ")) != NULL) {
				argcount++;
				argumente = realloc(argumente, sizeof(char *) * (argcount + 2));
			}

			if (argumente[0] == NULL);	
			else if (strcmp(argumente[0], "exit") == 0)
				exit(0);
			else if (strcmp(argumente[0], "copy") == 0)
				copy(&argcount, argumente);
			else if (strcmp(argumente[0], "run") == 0)
				runenv(&argcount, argumente);
			else if (strcmp(argumente[0], "cls") == 0)
				cls(&argcount, argumente);	
			else if (strcmp(argumente[0], "cd") == 0)
				cd(&argcount, argumente, cwdbuffer);
			else if (strcmp(argumente[0], "date") == 0)
				date(&argcount);
			else if (strcmp(argumente[0], "ls") == 0)
				list_dir(&argcount, argumente);
			else if (strcmp(argumente[0], "math") == 0)
				math(&argcount, argumente);
			else if (strcmp(argumente[0], "cwd") == 0)
				cwd(&argcount, argumente, dirbuffer, cwdbuffer);
			else if (strcmp(argumente[0], "mkdir") == 0)
				_mkdir(argumente[1]);
			else if (strcmp(argumente[0], "rmdir") == 0)
				_rmdir(argumente[1]);
			else if (strcmp(argumente[0], "echo") == 0)
				echo(&argcount, argumente);
			else 
				ERRORF(nocommand);
		} while (0);
	}
}

int main(int argc, char *argv[]) {
	WinShell();
	return 0;
}
