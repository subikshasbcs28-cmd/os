#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

// Struct definition for carrying marks data
struct message
{
    long msg_type;
    int n;
    int marks[10];
};

int main()
{
    key_t key;
    int msgid;
    struct message msg;
    pid_t pid;
    int i; // Loop variable upfront declaration for C89 compatibility

    // 1. Message Queue-kaga key ready panrom
    key = ftok("msgfile", 65);
    if (key == -1) {
        perror("ftok failed (Make sure 'msgfile' exists)");
        exit(1);
    }

    // 2. Message Queue segment create panrom
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed to create queue");
        exit(1);
    }

    // --- Input Section (Fork panrathuku munnadiye safe-ah input vangirom) ---
    msg.msg_type = 1;

    printf("--- Input Section ---\n");
    printf("Enter number of students: ");
    if (scanf("%d", &msg.n) != 1 || msg.n > 10) {
        fprintf(stderr, "Invalid number of students (max 10).\n");
        exit(1);
    }

    for(i = 0; i < msg.n; i++)
    {
        printf("Enter mark of student %d: ", i + 1);
        if (scanf("%d", &msg.marks[i]) != 1) {
            fprintf(stderr, "Invalid mark input.\n");
            exit(1);
        }
    }

    // 3. Ippo ready-ah irukura input-oda fork panrom
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    // ==========================================
    // CHILD PROCESS (CONSUMER)
    // ==========================================
    else if (pid == 0) {
        // Queue-la irunthu marks data-a receive panrom
        if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0) == -1) {
            perror("msgrcv failed to receive marks");
            exit(1);
        }

        printf("\n--- Child (Consumer) Context ---");
        printf("\n---------- GRADE SHEET ----------\n");
        printf("Student\tMark\tGrade\n");
        printf("-------------------------------\n");

        for(i = 0; i < msg.n; i++)
        {
            printf("%d\t%d\t", i + 1, msg.marks[i]);

            if(msg.marks[i] >= 90)
                printf("A+\n");
            else if(msg.marks[i] >= 80)
                printf("A\n");
            else if(msg.marks[i] >= 70)
                printf("B+\n");
            else if(msg.marks[i] >= 60)
                printf("B\n");
            else if(msg.marks[i] >= 50)
                printf("C\n");
            else if(msg.marks[i] >= 40)
                printf("D\n");
            else
                printf("F\n");
        }
        exit(0); // Child process pathu mudichutu successfully exit aagidum
    }

    // ==========================================
    // PARENT PROCESS (PRODUCER & CLEANUP)
    // ==========================================
    else {
        // Vanguna marks-a Message Queue vazhiya Child-ku anuprom
        if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("msgsnd failed to send marks");
            exit(1);
        }
        printf("Producer sent the marks data safely.\n");

        // Child process grade table sheet-a print panni mudikira varaikum Parent wait pannum
        wait(NULL);

        // 4. Kadaisiya OS kernel memory-la irunthu Message Queue-a pooranama remove panrom
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("msgctl RMID failed");
        }

        printf("\nParent cleaned up Message Queue. Exiting...\n");
    }

    return 0;
}
