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
        perror("msgget failed");
        exit(1);
    }

    if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0) == -1) {
        perror("msgrcv failed to receive marks");
        exit(1);
    }

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

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl RMID failed");
    }

    return 0;
}
