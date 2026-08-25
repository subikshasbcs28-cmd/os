#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

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

    printf("Enter a message: ");
    fgets(msg.msg_text, 100, stdin);

    if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
        perror("msgsnd failed to send message");
        exit(1);
    }

    printf("Message sent: %s", msg.msg_text);

    return 0;
}
