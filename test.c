#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void tst_error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
    exit(1);
}

int changeDir(char** comand)
{
    char* cdCommand = "cd";
    if(strcmp(comand[0], cdCommand) == 1)
    {
        if(comand[1] != NULL && comand[2] == NULL)
        {
            chdir(comand[1]);
            return 1;
        }
    }
    return 0;
}

//Function to print current directory
void printDirectory()
{
    char dir[1024];
    getcwd(dir, sizeof(dir));
    printf("\nDir: %s", dir);
}

char * tst_path(char *a[])
{
 char path[512];
 char wholename[512];
 char *next_piece;
 int counter = 0;
 int size = 0;
 int bcount = 0;
 
 //b = malloc(size, sizeof(string));

while(a[counter] != NULL)
{
    counter++;
}
size = counter;
char *b[size];
counter = 0;

 while(a[counter] != NULL)
{
    snprintf(path, 511, a[counter]);
    if (access(path, X_OK) == 0) 
    {
        //realloc(b, size);
        b[bcount] = a[counter];
        bcount++;
    }
    counter++; 
}
 return b;
}


int
main(int argc, char *argv[])
{
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    const char *path = "./";

    printf(">>");

    while ((nread = getline(&line, &len, stdin)) != -1) {

        
        //exit
        if(strcmp(line, "exit\n") == 0){
           exit(EXIT_SUCCESS);
        }

        char *commandTok = strtok(line, "&\n");
        char **commandArr = malloc(sizeof(char*));
        
        int commandCount = 0;
        while(commandTok != NULL){
            commandCount++;
            commandArr = realloc(commandArr,commandCount*sizeof(char*));
            commandArr[commandCount-1] = commandTok;
            printf("%d:%s\n",commandCount-1,commandTok);
            commandTok = strtok(NULL,"&\n");
        }

        int procressCount = 0;
        //printf("commandCount:%d\n", commandCount);
        for(int i = 0; i < commandCount; i++){
            int child = fork();
            if(child == 0){
                //printf("procress %d\n", procressCount);
                //printf("content: %s\n", commandArr[procressCount]);
                char *whole = commandArr[procressCount];
                char **arg = malloc(sizeof(char*));
                char *argTok = strtok(whole," ");
                int currindex = 0;
                while(argTok != NULL){
                    arg[currindex] = argTok;
                    printf("%d - arg %d: %s\n", getpid(), currindex, argTok);
                    currindex++;
                    arg = realloc(arg, (currindex+1) * sizeof(char*));
                    argTok = strtok(NULL, " ");
                }
                arg[currindex] = NULL;
                exit(0);
            }
            else{

                procressCount++;
            }
        }

        for(int i=0; i < commandCount; i++){
            wait(NULL);
        }
        
    printf(">>");
    }


    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}


