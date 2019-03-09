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

        //exit
        if(strcmp(line, "exit\n") == 0){
            exit(EXIT_SUCCESS);
        }

        //fork
        int child = fork();

        //argument array
        char *arg[3];
        int counter = 0;

        //split up getline by space and remove \n character
        char * split = strtok(line, "\n ");
        
        //put the space seprated arguments into the array
        while(counter <= 3&& split != NULL){
            arg[counter] = strdup(split);
            split = strtok (NULL, "\n ");
            counter++;
        }

        //append NULL to signify end of array
        arg[counter] = NULL;

        if(child == 0){

            //launch with argument array
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


