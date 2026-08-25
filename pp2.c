#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// Message structuring setup
struct message
{
    long msg_type;
    char msg_text[100];
};

int main()
{
    key_t key;
    int msgid;
    struct message msg;
    pid_t pid;

    // Child variables
    int i;
    int count = 0;
    int in_word = 0;

    // 1. Message Queue Key-a generate panrom
    key = ftok("msgfile", 65);
    if (key == -1) {
        perror("ftok failed (Make sure 'msgfile' exists)");
        exit(1);
    }

    // 2. Message Queue segment-a create panrom
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed to create queue");
        exit(1);
    }

    // --- Input Section (Fork-ku munnadiye input vaangi terminal-a safe panrom) ---
    msg.msg_type = 1;
    printf("--- Input Section ---\n");
    printf("Enter a message: ");
    fgets(msg.msg_text, 100, stdin);

    // 3. Ippo fork panni Parent & Child-a pirikirom
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    // ==========================================
    // CHILD PROCESS (CONSUMER)
    // ==========================================
    else if (pid == 0) {
        // Child context-la message queue-la irunthu data-a receive panrom
        if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 1, 0) == -1) {
            perror("msgrcv failed to receive message");
            exit(1);
        }

        printf("\n--- Child (Consumer) Context ---\n");
        printf("Consumer received: %s", msg.msg_text);

        // Word count Logic (C89 loop compatible)
        for (i = 0; msg.msg_text[i] != '\0'; i++)
        {
            if (msg.msg_text[i] != ' ' &&
                msg.msg_text[i] != '\n' &&
                msg.msg_text[i] != '\t')
            {
                if (in_word == 0)
                {
                    count++;
                    in_word = 1;
                }
            }
            else
            {
                in_word = 0;
            }
        }

        printf("Number of words: %d\n", count);
        exit(0); // Child process end
    }

    // ==========================================
    // PARENT PROCESS (PRODUCER & CLEANUP)
    // ==========================================
    else {
        // Parent context-la message-a queue-ku send panrom
        if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd failed to send message");
            exit(1);
        }
        printf("Producer sent the message safely.\n");

        // Child process thannoda word count calculation-a mudikira varaikum wait panrom
        wait(NULL);

        // 4. Kadaisiya OS kernel-la irunthu Message Queue-a delete panrom
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("msgctl RMID failed");
        }

        printf("\nParent cleaned up Message Queue. Exiting...\n");
    }

    return 0;
}
