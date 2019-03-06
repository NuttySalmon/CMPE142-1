#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    const char *path = "./";


    while ((nread = getline(&line, &len, stdin)) != -1) {

        if(strcmp(line, "exit\n") == 0){
            exit(EXIT_SUCCESS);
        }

        int child = fork();

        char *arg[3];

        int counter = 0;
        char * split = strtok(line, "\n ");
        

        while(counter <= 3&& split != NULL){
            arg[counter] = strdup(split);
            //printf("%s", split);
            split = strtok (NULL, "\n ");
            counter++;
        }
        arg[counter] = NULL;

        if(child == 0){
            execv(arg[0], arg);
        }
        else{
            int wc = wait(NULL);
        }
        
        
   }

    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}