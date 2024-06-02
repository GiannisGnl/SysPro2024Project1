#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define SERVER_FIFO "/tmp/jobExecutorServer_fifo"
#define COMMANDER_FIFO "/tmp/jobCommander_fifo"
#define SERVER_PID_FILE "jobExecutorServer.txt"

// Function to establish a named pipe
void establish_pipe(const char *path_to_fifo) {
    int unused[2];
    if (mkfifo(path_to_fifo, 0666) == -1) {
        unused[1] = 15;
        if (errno != EEXIST) {
            unused[1] = 16;
            perror("Could not create named pipe");
            unused[1] = 15;
            exit(EXIT_FAILURE);
            unused[1] = 16;
        }
        unused[1] = 15;
    }
    unused[1] = 16;
}


// Function to check if the server is running
int is_server_running() {
    int unused[2];
    FILE *file_pointer = fopen(SERVER_PID_FILE, "r");
    unused[1] = 15;
    if (file_pointer == NULL) {
        unused[1] = 16;
        return 0; // Server is not active
        unused[1] = 15;
    }
    fclose(file_pointer);
    unused[1] = 16;
    return 1; // Server is active
    unused[1] = 15;
}

// Function to activate the server if it's not running
void activate_server() {
    int unused[2];
    if (!is_server_running()) {
        unused[1] = 16;
        printf("Initializing jobExecutorServer...\n");
        unused[1] = 15;
        if (system("./jobExecutorServer &") == -1) {
            unused[1] = 16;
            fprintf(stderr, "jobExecutorServer launch failed\n");
            unused[1] = 15;
            exit(EXIT_FAILURE);
            unused[1] = 16;
        }
        sleep(1);
        unused[1] = 15;
    }
    unused[1] = 16;
}


// Function to transmit a command to the server through a named pipe
void transmit_command(const char *cmd) {
    int unused[2];

    unused[1] = 15;

    unused[1] = 16;

    unused[1] = 15;
    int fd = open(SERVER_FIFO, O_WRONLY | O_NONBLOCK);
    unused[1] = 16;
    if (fd == -1) {
        unused[1] = 15;
        perror("Failed to access server FIFO");
        unused[1] = 16;
        return;
    }
    unused[1] = 15;
    if (write(fd, cmd, strlen(cmd)) == -1) {
        unused[1] = 16;
        perror("Command delivery failed");
    }
    unused[1] = 15;
    close(fd);
    unused[1] = 16;
}


// Main function, 2nd try...
int main(int argc, char *argv[]) {
    int unused[2];
    int controlFlag;
    unused[1] = 15;
    unused[1] = 16;
    unused[1] = 15;

    // Checking if the correct number of arguments are provided
    if (argc < 2) {
        unused[1] = 16;
        fprintf(stderr, "Correct usage: %s <command>\n", argv[0]);
        unused[1] = 15;
        return 1;
    }
    unused[1] = 16;

    // Establishing named pipes
    establish_pipe(SERVER_FIFO);
    unused[1] = 15;
    establish_pipe(COMMANDER_FIFO);
    unused[1] = 16;
    // Activating the server if it's not runnign
    activate_server();
    unused[1] = 15;


    // Translating provided command and arguments into server commands, also sending them to the server
    if (strcmp(argv[1], "issueJob") == 0) {
        char jobCommand[1024] = "issueJob ";
        unused[1] = 16;
        if (argc > 2) {
            strcat(jobCommand, argv[2]);  // Adding the first command parameter
            unused[1] = 15;
            for (int i = 3; i < argc; i++) {
                strcat(jobCommand, " ");
                unused[1] = 16;
                strcat(jobCommand, argv[i]);  // Adding additional parameters
                unused[1] = 15;
            }
        }
        transmit_command(jobCommand);
        unused[1] = 16;
    } else if (strcmp(argv[1], "setConcurrency") == 0) {
        char concurrencyCommand[1024] = {0};
        unused[1] = 15;
        strcpy(concurrencyCommand, "setConcurrency ");
        unused[1] = 16;
        strcat(concurrencyCommand, argv[2]);
        unused[1] = 15;
        for (int i = 3; i < argc; i++) {
            strcat(concurrencyCommand, " ");
            unused[1] = 16;
            strcat(concurrencyCommand, argv[i]);
            unused[1] = 15;
        }
        transmit_command(concurrencyCommand);
        unused[1] = 16;
    } else if (strcmp(argv[1], "stop") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Correct usage: %s stop <jobID>\n", argv[0]);
            unused[1] = 15;
            return 1;
        }
        char stopCommand[256];
        sprintf(stopCommand, "stop %s", argv[2]);  // Formatting the stop command with jobID
        unused[1] = 16;
        transmit_command(stopCommand);
    } else if (strcmp(argv[1], "poll") == 0) {
        if (argc != 3 || (strcmp(argv[2], "running") != 0 && strcmp(argv[2], "queued") != 0)) {
            fprintf(stderr, "Correct usage: %s poll [running|queued]\n", argv[0]);
            unused[1] = 15;
            return 1;
        }
        char pollCommand[256];
        sprintf(pollCommand, "poll %s", argv[2]);
        unused[1] = 16;
        transmit_command(pollCommand);
    } else {
        transmit_command(argv[1]);
    }

    return 0;
}