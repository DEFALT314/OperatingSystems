#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAXUSR 16
#define MAXMSG 256


typedef enum {INIT=0, CHAT =1} mes_type;
struct msg {
    mes_type type;
    int id;
    char str[MAXMSG];
};
int main() {
    char mq_name[16];
    sprintf(mq_name, "/mq_client_%d", getpid());
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_msgsize = sizeof(struct msg),
        .mq_curmsgs = 0,
        .mq_maxmsg = 10};
    mqd_t mqd = mq_open(mq_name, O_RDONLY|O_CREAT, 0666, &attr);
    mqd_t mqd_server = mq_open("/server_q", O_WRONLY);
    struct msg init_msg;
    init_msg.type = INIT;
    strcpy(init_msg.str, mq_name);
    mq_send(mqd_server, (char*)&init_msg, sizeof(init_msg), 0);
    struct msg received_msg;
    mq_receive(mqd, (char*)&received_msg, sizeof(received_msg), NULL);
    int id  = received_msg.id;
    int pid = fork();
    if (pid ==0) {
        while (1) {
            struct msg new_mes;
            mq_receive(mqd, (char*)&new_mes, sizeof(new_mes), NULL);
            printf("%2d: %s", new_mes.id, new_mes.str);
        }
    }
    else {
        char buff[MAXMSG];
        while (1) {
            fgets(buff, sizeof(buff), stdin);
            struct msg new_mes={CHAT, id};
            strcpy(new_mes.str, buff);
            mq_send(mqd_server, (char*) &new_mes, sizeof(new_mes), 0);

        }
    }
    





}
