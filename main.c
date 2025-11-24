#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    
    if(argc < 3){
        fprintf(stderr,"Usage: %s priority_level command_to_execute -arg1 -arg2 ... -argn\n", argv[0]);
        return 1;
    }

    int nice_value = atoi(argv[1]);
    char **command = &argv[2];

    if(nice_value < -20 || nice_value > 19){
        fprintf(stderr,"Priority level must be between -20 and 19.\n");
        return 1;
    }

    pid_t pid = fork();

    if(pid < 0) {
        perror("Fork failed");
        return 1;
    }

    else if(pid == 0){
        if(setpriority(PRIO_PROCESS, 0, nice_value) == -1){
            perror("Failed to set priority");
            return 1;
        }

        else{
            execvp(command[0], command);
            perror("Execution failed");
            return 127;
        }
    }

    int status;
    if(waitpid(pid, &status, 0) == -1){
        perror("Waitpid failed");
        return 1;
    }

    if(WIFEXITED(status)){
        return WEXITSTATUS(status);
    }
    else{
        fprintf(stderr, "Child process did not terminate normally.\n");
        return 1;
    }

    return 1;
}
