#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char **path = malloc(3* sizeof(char*));

    path[0] = "./";
    path[1] = "/bin";
    path[2] = NULL;

    printDirectory();
    printf(">> ");

    while ((nread = getline(&line, &len, stdin)) != -1) {

        
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

            //fork
            int child = fork();

            //child procress
            if(child == 0){

                char **arg = split(commandArr[procressCount], " :", NULL);
                printf("size: %zd", sizeof(arg)/sizeof(char*));
               
                if(strcmp(arg[0], "exit") == 0){
                    tst_error(); //error if did not exit because more than 0 arguments

                } else if(strcmp(arg[0], "path") == 0){

                    //reserved for "path"

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
                exit(0);
            }
            else{

                procressCount++;
            }
        }

        for(int i=0; i < commandCount; i++){
            wait(NULL);
        }
        free(commandArr);
    
    printDirectory();
    printf(">> ");
    }


    free(path);
    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}


