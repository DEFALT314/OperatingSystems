#include <errno.h>
#include <mqueue.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXUSR 16
#define MAXMSG 256

typedef enum {INIT=0, CHAT =1} mes_type;
struct msg {
    mes_type type;
    int id;
    char str[MAXMSG];
};
int main(){
    mqd_t usr_qd[MAXUSR];
    mq_unlink("/server_q");
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_msgsize = sizeof(struct msg),
        .mq_curmsgs = 0,
        .mq_maxmsg = 10};
    mqd_t qd = mq_open("/server_q", O_RDONLY|O_CREAT,0666, &attr );
    int cur_i = 0;
    struct msg recived_msg;
    unsigned int prio;
    while (1) {
        ssize_t read_b = mq_receive(qd, (char *)&recived_msg, sizeof(recived_msg), &prio);
        if (recived_msg.type == INIT) {
            usr_qd[cur_i] = mq_open(recived_msg.str, O_WRONLY);
            struct msg reply_msg = {.id = cur_i, .type = INIT};
            mq_send(usr_qd[cur_i], (char*)&reply_msg, sizeof(reply_msg), 0 );
            printf("New user: %d\n", cur_i);
            cur_i++;
        }
        else if (recived_msg.type == CHAT) {
            for (int i = 0; i<cur_i; i++) {
                if (i!= recived_msg.id) {
                    mq_send(usr_qd[i], (char*)&recived_msg, sizeof(recived_msg), 0);
                }
            }
        }

    }



}