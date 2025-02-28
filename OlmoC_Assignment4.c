#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define INPUT_LENGTH 2048
#define MAX_ARGS 512

int last_status = 0; // global variable to store the last status of a process

struct command_line {
    char *argv[MAX_ARGS + 1];
    int argc;
    char *input_file;
    char *output_file;
    bool is_bg;
};

struct command_line *curr_command;
/*
void handle_SIGINT(int signo){
    if(curr_command->is_bg == false){
        printf("\n");
        fflush(stdout);
    }
  }

void handle_SIGCHLD(int signo) {
    int child_status;
    pid_t child_pid;
    while ((child_pid = waitpid(-1, &child_status, WNOHANG)) > 0) {
        printf("Background pid %d is done: terminated by signal %d\n", child_pid, WEXITSTATUS(child_status));
        fflush(stdout);
    }
}  
*/
//This code was adapted from CS344's Assignment 4 provided code
struct command_line *parse_input() {
    char input[INPUT_LENGTH];
    struct command_line *curr_command = (struct command_line *) calloc(1, sizeof(struct command_line));

    // Get input
    printf(": ");
    fflush(stdout);
    fgets(input, INPUT_LENGTH, stdin);
    // Tokenize the input
    char *token = strtok(input, " \n");
    while(token) {
        if(!strcmp(token, "<")) {
            curr_command->input_file = strdup(strtok(NULL, " \n"));
        } else if(!strcmp(token, ">")) {
            curr_command->output_file = strdup(strtok(NULL, " \n"));
        } else if(!strcmp(token, "&")) {
            curr_command->is_bg = true;
        } else {
            curr_command->argv[curr_command->argc++] = strdup(token);
        }
        token = strtok(NULL, " \n");
    }
    return curr_command;
}

void free_command(struct command_line *command) {
    for (int i = 0; i < command->argc; i++) {
        free(command->argv[i]);
    }
    if (command->input_file) {
        free(command->input_file);
    }
    if (command->output_file) {
        free(command->output_file);
    }
    free(command);
}

void spawn_child(struct command_line *cmd){
    pid_t spawnpid = fork();
    switch (spawnpid) {
        case -1:
            perror("fork() failed");
            break;
        case 0:
            // Child process
            if (cmd->input_file) {
                FILE *input = fopen(cmd->input_file, "r");
                if (!input) {
                    perror("fopen() failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fileno(input), STDIN_FILENO);
                fclose(input);
            }
            if (cmd->output_file) {
                FILE *output = fopen(cmd->output_file, "w");
                if (!output) {
                    perror("fopen() failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fileno(output), STDOUT_FILENO);
                fclose(output);
            }
            execvp(cmd->argv[0], cmd->argv);
            perror("execvp() failed");
            exit(EXIT_FAILURE);
        default:
            // Parent process
            if (!cmd->is_bg) {
                int child_status;
                waitpid(spawnpid, &last_status, 0);
            } else {
                printf("Background pid is %d\n", spawnpid);
            }
    }
}

int main() {

    /* set up for the sigint handler
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = handle_SIGINT;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);

    // set up for sigchld handler
    struct sigaction SIGCHLD_action = {0};
    SIGCHLD_action.sa_handler = handle_SIGCHLD;
    sigfillset(&SIGCHLD_action.sa_mask);
    SIGCHLD_action.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &SIGCHLD_action, NULL);
    */

    while(true) {
        curr_command = parse_input();
        
        // Check if curr_command->argv[0] is not NULL before accessing it
        if (curr_command->argc > 0 && curr_command->argv[0] != NULL) {
            if (!strcmp(curr_command->argv[0], "exit")) {
                return EXIT_SUCCESS;
            } else if (curr_command->argv[0][0] == '#') {
                //if the first value is a #, ignore that line
                continue;
            } else if (!strcmp(curr_command->argv[0], "cd")) {
                // Change to the home directory if no arguments were sent
                if (curr_command->argc == 1) {
                    chdir(getenv("HOME"));
                } else {
                    // Change to the specified directory
                    chdir(curr_command->argv[1]);
                }
            } else if (!strcmp(curr_command->argv[0], "status")) {
                // Print the exit status or the terminating signal of the last foreground process
                printf("Exit value %d\n", WEXITSTATUS(last_status));
                fflush(stdout);
            } else {
                // Execute other commands and spawn child processes
                spawn_child(curr_command);
            }
        }
        
        // Free the allocated memory for the command
        free_command(curr_command);
    }
    return EXIT_SUCCESS;
}
