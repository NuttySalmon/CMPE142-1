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
        if(comand[1] != NULL && comand[2] == NULL && access(comand[1], R_OK)==0)
        {
            chdir(comand[1]); 
            return 1;
        }
        else
        {
            printf("Directory not found : %s\n", comand[1]);
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



char ** tst_path(char *a[])
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
    char **b = malloc(sizeof(char*));
    counter = 0;

    while(a[counter] != NULL)
    {
        snprintf(path, 511, a[counter]);
        snprintf(wholename,511,"%s/ls", path);
        if (access(wholename, X_OK) == 0) // access(?) execv(?)
        {
            //execv(wholename);
            b[bcount] = a[counter];
            b = realloc(b,(bcount+1) * sizeof(char*));
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

char* removeSpaces(char* s)
{
    char* cpy = s;  // an alias to iterate through s without moving s
    char* temp = s;

    while (*cpy)
    {
        if (*cpy != ' ' && *cpy != '\t')
            *temp++ = *cpy;
        cpy++;
    }
    *temp = 0;

    //printf("wo white space :%s\n", s); // This prints out the desired result: abbcccd
    return s;
}

/*
free each element in array that is dynamically allocated
*/
void freeArr(char** s){
    for(int i = 0; s[i] != NULL; i++){
        free(s[i]);
    }
    free(s);
}

int
main(int argc, char *argv[])
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    FILE* input; //input source

    char **paths  = malloc(2 * sizeof(char*));

    //default paths
    paths[0] = strdup("/bin");
    paths[1] = NULL;

    //check if batch file exists
    if(access(argv[1], R_OK) == 0){
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

        int commandCount; //number of commands
        char **commandArr = split(line, "&\n", &commandCount); //create array of commands
        int procressCount = 0; //number of processes called

        //traverse array of commands
        for(int i = 0; i < commandCount; i++){
            int redirCount = 0;
            //parse redirection
            char **redir = split(commandArr[procressCount], ">", &redirCount); 

            char **arg = split(redir[0], " :\t", NULL); //create array of arg
            int child = -1;
            
            if(strcmp(arg[0], "cd") == 0){
                changeDir(arg);     //change directory                
            } 
            else if(strcmp(arg[0], "path") == 0){
                printf("changing path\n");
                freeArr(paths);
                paths = tst_path(arg);   // change path
                printf("path: %s\n",paths[0]);

            } 
            else{
                 child = fork();
            }


            //child procress
            if(child == 0){

                if(redirCount == 2){
                    char * output = removeSpaces(redir[1]);
                    close(STDOUT_FILENO); 
                    open(output, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                }
               // printf("size: %zd", sizeof(arg)/sizeof(char*));
               
                if(strcmp(arg[0], "exit") == 0){
                    tst_error(); //error if did not exit because more than 0 arguments

                } else if (strcmp(arg[0], "cd") == 0 ){

                    tst_error();

                } 

                //exec
                else{
                    
                    char* wholename;

                    //traverse array of paths
                    for(int i = 0; paths[i] != NULL; i++){

                        //contruct full path of exe
                        wholename = malloc(sizeof(paths[i]) + sizeof(arg[0]) + 2);
                        strcpy(wholename, paths[i]);
                        strcat(wholename,"/");
                        strcat(wholename, arg[0]);

                        if (access(wholename, X_OK) == 0){  //check if execeutable
                            execv(wholename,arg);   //run
                            break;
                        }
                        
                    }
                    free(wholename);
                } 
                free(arg);
                free(redir);
                exit(0);
            }
            else{
                free(arg);
                free(redir);
                procressCount++; //increment no. of process called
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

    freeArr(paths);
    free(line);
    exit(EXIT_SUCCESS);
}


