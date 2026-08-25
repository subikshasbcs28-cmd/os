#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#includ <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    key_t key;
    int shmid;
    char *shared_memory;
    pid_t pid;
    int i;
    int length;

    key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }

    printf("--- Input Section ---\n");
    printf("Enter a string: ");
    fgets(shared_memory, 1024, stdin);
    printf("Producer wrote: %s\n", shared_memory);

    shmdt(shared_memory);

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0) {
        shared_memory = (char *)shmat(shmid, NULL, 0);

        length = strlen(shared_memory);
        if (length > 0 && shared_memory[length - 1] == '\n')
            length--;

        printf("--- Child (Consumer) Context ---\n");
        printf("Consumer read: %s\n", shared_memory);

        printf("Reversed string: ");
        for (i = length - 1; i >= 0; i--) {
            printf("%c", shared_memory[i]);
        }
        printf("\n");

        shmdt(shared_memory);
        exit(0);
    }
    else {
        wait(NULL);

        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl RMID failed");
        }

        printf("\nParent cleaned up Shared Memory. Exiting...\n");
    }

    return 0;
}
