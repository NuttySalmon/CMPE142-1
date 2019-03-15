#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void tst_error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
    exit(1);
}

int changeDir(char** comand)
{
    char* cdCommand = "cd";
    if(strcmp(comand[0], cdCommand) == 0) 
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
    printf("\033[01;33m");
    printf("%s", dir);
    printf("\033[0m");
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
        snprintf(wholename,511,"%s/ls", path);
        if (access(wholename, X_OK) == 0) // access(?) execv(?)
        {
            //realloc(b, size);
            //execv(wholename);
            b[bcount] = a[counter];
            bcount++;
        }
        counter++; 
    }
     return b; //should return char** which is array of strings
}


char** split(char* orig, char* delim, int* counter){
    char **arr = malloc(sizeof(char*));
    char *tok = strtok(orig, delim); //parse arguments
    int currindex = 0;

    //traverse token 
    while(tok != NULL){
        arr[currindex] = tok;
        // printf("%d - arg %d: %s\n", getpid(), currindex, argTok);
        currindex++;
        arr = realloc(arr, (currindex+1) * sizeof(char*));
        tok = strtok(NULL, delim);
    }
    
    arr[currindex] = NULL;
    if (counter != NULL)
        *counter = currindex;
    return arr;
}


int
main(int argc, char *argv[])
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    FILE* input;
    char **path = malloc(3* sizeof(char*));

    path[0] = "./";
    path[1] = "/bin";
    path[2] = NULL;

    if(access(argv[1], R_OK) == 0){
        //close(stdin);
       // printf("%s", argv[1]);
        input = fopen(argv[1], "r");

    } else{
        input = stdin;
        printDirectory();
        printf(" >> ");

    }


    while ((nread = getline(&line, &len, input)) != -1) {
        
        //exit
        if(strcmp(line, "exit\n") == 0){
           exit(EXIT_SUCCESS);
        }


        int commandCount;
        char **commandArr = split(line, "&\n", &commandCount);
        
        int procressCount = 0;
        //printf("commandCount:%d\n", commandCount);
        

        //traverse array of commands
        for(int i = 0; i < commandCount; i++){

            int redirCount = 0;
            char **redir = split(commandArr[procressCount], ">", &redirCount);
            //char * cmd = redir[0];

            //printf("%d redir: %s\n", redirCount, redir[0]);
            
            char **arg = split(redir[0], " :\t", NULL);

            int child = 1;

            if(strcmp(arg[0], "cd") == 0){
                changeDir(arg);           
            }
            else{
                //fork
                 child = fork();
            }


            //child procress
            if(child == 0){

                if(redirCount == 2){
                    close(STDOUT_FILENO); 
                    open(redir[1], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                }
               // printf("size: %zd", sizeof(arg)/sizeof(char*));
               
                if(strcmp(arg[0], "exit") == 0){
                    tst_error(); //error if did not exit because more than 0 arguments

                } else if(strcmp(arg[0], "path") == 0){

                    //reserved for "path"

                } else if (strcmp(arg[0], "cd") == 0 ){

                    tst_error();

                } else{

                    //exec

                    char* wholename;

                    //traverse array of paths
                    for(int i = 0; path[i] != NULL; i++){

                        wholename = malloc(sizeof(path[i]) + sizeof(arg[0]) + 2);
                        strcpy(wholename, path[i]);
                        strcat(wholename,"/");
                        strcat(wholename, arg[0]);

                        if (access(wholename, X_OK) == 0){
                            execv(wholename,arg);
                            free(wholename);
                            break;
                        }
                        free(wholename);
                    }
                } 
                free(arg);
                free(redir);
                exit(0);
            }
            else{
                free(arg);
                free(redir);
                procressCount++;
            }
        }

        for(int i=0; i < commandCount; i++){
            wait(NULL);
        }
        free(commandArr);
        if(input == stdin){
            printDirectory();
            printf(" >> ");
        }
    }


    free(path);
    free(line);
    exit(EXIT_SUCCESS);
}


