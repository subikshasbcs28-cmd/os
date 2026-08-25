#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message
{
    long msg_type;
    int n;
    int marks[10];
};

int main()
{
    key_t key;
    int msgid, i;
    struct message msg;

    key = ftok("msgfile", 65);
    if (key == -1) {
        perror("ftok failed (make sure 'msgfile' exists)");
        exit(1);
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed to create queue");
        exit(1);
    }

    msg.msg_type = 1;

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

    if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
        perror("msgsnd failed to send marks");
        exit(1);
    }

    printf("Marks sent successfully.\n");

    return 0;
}
