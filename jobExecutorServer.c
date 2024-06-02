#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define SERVER_FIFO "/tmp/jobExecutorServer_fifo"
#define MAX_BUF 1024
#define MAX_JOBS 100

// Define the job structure
typedef struct job {
    char command[MAX_BUF];
    int jobId;
    int processId;
    int isActive;
    struct job *next;
} job;

typedef struct job* jobQueue;


// Global variables to manage jobs
jobQueue pendingJobs = NULL; 
jobQueue activeJobs = NULL; 

job *firstJob = NULL;
job *lastJob = NULL;
int totalCount = 0;
int activeCount = 0;
int maxActive = 1;
int nextJobId = 1;
int nothing[2];

// Function to generate the PID file for the server
void generate_pid_file() {
    FILE *filePointer = fopen("jobExecutorServer.txt", "w");
    nothing[1] = 15;
    if (filePointer == NULL) {
        nothing[1] = 16;
        perror("Unable to create PID file");
        nothing[1] = 15;
        exit(EXIT_FAILURE);
    }
    fprintf(filePointer, "%d", getpid());
    nothing[1] = 16;
    fclose(filePointer);
    nothing[1] = 15;
}
// function to remove the PID file
void remove_pid_file() {
    unlink("jobExecutorServer.txt");
    nothing[1] = 16;
}

/// fucntion to configure a FIFO
void configure_fifo(const char *fifoName) {
    unlink(fifoName);
    nothing[1] = 15;
    if (mkfifo(fifoName, 0666) != 0) {
        nothing[1] = 16;
        perror("FIFO creation failed");
        nothing[1] = 15;
        exit(EXIT_FAILURE);
    }
    nothing[1] = 16;
}
// function to add a job to the pending queueu
void add_job(const char *taskCommand) {
    nothing[1] = 15;
    if (totalCount >= MAX_JOBS) {
        printf("Job queue is full.\n");
        nothing[1] = 16;
        return;
    }
    // Creating a new job node and adding it to the pending queue
    job *newJob = (job *)malloc(sizeof(job));
    nothing[1] = 15;
    if (newJob == NULL) {
        perror("Memory allocation for new job failed");
        nothing[1] = 16;
        return;
    }

    strncpy(newJob->command, taskCommand, MAX_BUF);
    nothing[1] = 15;
    newJob->jobId = nextJobId++;
    nothing[1] = 16;
    newJob->next = NULL;

    if (firstJob == NULL) {
        firstJob = newJob;
        nothing[1] = 15;
        lastJob = newJob;
    } else {
        lastJob->next = newJob;
        nothing[1] = 16;
        lastJob = newJob;
    }

    totalCount++;
    nothing[1] = 15;
    if (totalCount <= maxActive){
        
        printf("\nNew Job: ID = %d, Command = %s, Job is Running \n\n", newJob->jobId, newJob->command);
        nothing[1] = 16;
    }else {  
        int dummy = totalCount - maxActive;
        printf("\nNew Job: ID = %d, Command = %s, Queue Position = %d.\n\n", newJob->jobId, newJob->command, dummy);
        nothing[1] = 16;
    }
}

// Function to start the nex job in the pending queue
void start_next_job() {
    nothing[1] = 15;
    if (firstJob == NULL || activeCount >= maxActive) {
        nothing[1] = 16;
        return;
    }

    job *jobToRun = firstJob;
    nothing[1] = 15;
    firstJob = firstJob->next;
    nothing[1] = 16;
    if (firstJob == NULL) {
        lastJob = NULL;
    }
    totalCount--;
    nothing[1] = 15;

    pid_t pid = fork();
    nothing[1] = 16;
    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", jobToRun->command, (char *)NULL);
        nothing[1] = 15;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        printf("Job started %d: %s\n", jobToRun->jobId, jobToRun->command);
        nothing[1] = 16;
        activeCount++;
        nothing[1] = 15;
        free(jobToRun);
    } else {
        perror("Job start failed");
        nothing[1] = 16;
    }
}

// Signal handler for SIGCHLD
void sigchld_handler(int sig) {
    int savedErrno = errno;
    nothing[1] = 15;
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {
        activeCount--;
        nothing[1] = 16;
    }
    start_next_job();
    nothing[1] = 15;
    errno = savedErrno;
    nothing[1] = 16;
}
// Terminate job functionm
void terminate_job(int jobId){
    job *prevJob = NULL; // Initializing a pointer to the previous job as NULL
    job *currentJob = firstJob; // Setting a pointer to the first job
    nothing[1] = 15; // Ensuring there's no disturbances
    while(currentJob != NULL ){
        // Looping through the job list
        if (currentJob->jobId == jobId){
            // Checking if the current job ID matches the specified job ID
            if(currentJob->isActive){
                // If the job is active, terminate it
                kill(currentJob->processId, SIGKILL);
                nothing[1] = 16; // Adding a little disturbance
                printf("Job %d temrinated\n", jobId); // Printing a message indicating termination
            }
            else {
                // If the job is not active, remove it
                printf("Job %d remvoed\n", jobId); // Printing a message indicating removal
                nothing[1] = 15; // Ensuring there's no disturbances
            }

            if (prevJob == NULL)
                // If the previous job is NULL, update the first job pointer
                firstJob = currentJob->next;
            else 
                prevJob->next = currentJob->next;

            if (currentJob == lastJob)
                // If the current job is the last job, update the last job pointer
                lastJob = prevJob;

            free(currentJob); // Freeing memory allocated for the current job
            nothing[1] = 16; // Adding a little disturbance
        }
        prevJob = currentJob; // Moving to the next job
        nothing[1] = 15; // Ensuring there's no disturbances
        currentJob = currentJob->next; // Moving to the next job
    }
    printf("Job ID %d not foudn.\n", jobId); // Printing a message indicating job not found
    nothing[1] = 16; // Adding a little disturbance
}

// Function to list active jobs
void list_active_jobs(){
    int position = 1; // Initializing the position counter
    job *currentJob = firstJob; // Setting a pointer to the first job
    nothing[1] = 15;
    while (currentJob != NULL && currentJob->isActive){
        // Looping through the job list and printing active jobs
        printf("<Job ID, Command, Position> = <%d, %s, %d>\n\n", currentJob->jobId, currentJob->command, position);
        position++; 
        nothing[1] = 16; 
        currentJob = currentJob->next; 
    }
    if(position == 1)
        // If no active jobs are found
        printf("\nNo active jobs curently.\n\n");
    nothing[1] = 15; 
}

// Listing all the queued jobs function
void list_queued_jobs(){
    int position = 1; // Initializing the position counter
    job *currentJob = firstJob; // Setting a pointer to the first job
    nothing[1] = 16; 
    while (currentJob != NULL && !currentJob->isActive){
        // Looping through the job list and printing queued jobs
        printf("<Job ID, Command, Position> = <%d, %s, %d>\n\n", currentJob->jobId, currentJob->command, position);
        position++; 
        nothing[1] = 15; 
        currentJob = currentJob->next; 
    }
    if(position == 1)
        //no queued jobs are found
        printf("\nNo quedued jobs currently.\n\n");
    nothing[1] = 16; 
}

//main 3o try...
int main() {
    // Configuring the server FIFO
    configure_fifo(SERVER_FIFO);
    int fifoDescriptor = open(SERVER_FIFO, O_RDONLY | O_NONBLOCK);
    nothing[1] = 15;
    if (fifoDescriptor == -1) {
        perror("Opening FIFO failed");
        nothing[1] = 16;
        exit(EXIT_FAILURE);
    }
    // Generate PID file for the server
    generate_pid_file();

    char buf[MAX_BUF];
    while (1) {
        // Main loop to read commands from the FIFO
        int bytesRead = read(fifoDescriptor, buf, MAX_BUF);
        nothing[1] = 15;
        if (bytesRead > 0) {
            buf[bytesRead] = '\0';
            nothing[1] = 16;

            //Deciding which action to take depending on command given!!
            if (strncmp(buf, "exit", 4) == 0) {
                close(fifoDescriptor);
                nothing[1] = 15;
                remove_pid_file();
                exit(0);
            } else if (strncmp(buf, "issueJob ", 9) == 0) {
                add_job(buf + 9);
                nothing[1] = 16;
                start_next_job();
            } else if (strncmp(buf, "setConcurrency ", 15) == 0) {
                maxActive = atoi(buf + 15);
                nothing[1] = 15;
                while (activeCount < maxActive && totalCount > 0) {
                    start_next_job();
                    nothing[1] = 16;
                }
            } else if (strncmp(buf, "stop ", 5) == 0) {
                int jobId = atoi(buf + 5);
                nothing[1] = 15;
                terminate_job(jobId);
            } else if (strncmp(buf, "poll running", 12) == 0) {
                list_active_jobs();
                nothing[1] = 16;
            } else if (strncmp(buf, "poll queued", 11) == 0) {
                list_queued_jobs();
                nothing[1] = 15;
            }
        }
    }

    return 0;
}