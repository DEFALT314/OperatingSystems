#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define TEXT_LENGTH 10
#define QUEUE_SIZE 10
typedef struct {
    char text[QUEUE_SIZE][TEXT_LENGTH];
    int head;
    int tail;
} Queue;


int main(){
    Queue *q;
    key_t shm_key = ftok("/tmp", 'A');
    int shm_id = shmget(shm_key, sizeof(Queue), IPC_CREAT | IPC_EXCL | 0666);
    if ( shm_id !=-1) {
        q = shmat(shm_id, NULL, 0);
        q->head =0;
        q->tail = 0;
    }
    else {
        shm_id = shmget(shm_key, sizeof(Queue), 0666);
        q = shmat(shm_id, NULL, 0);
    }

    key_t sem_key = ftok("/tmp", 'B');
    int sem_id = semget(sem_key, 3, IPC_EXCL | IPC_CREAT | 0666);
    if ( sem_id !=-1) {
        semctl(sem_id, 0, SETVAL,1);
        semctl(sem_id, 1, SETVAL,0);
        semctl(sem_id, 2, SETVAL,QUEUE_SIZE);
    }
    else {
        sem_id = semget(sem_key, 3, 0666);
    }

    char buff[TEXT_LENGTH];
    struct sembuf post_empty = {.sem_num = 2, .sem_op = 1, .sem_flg = 0};
    struct sembuf wait_full = {.sem_num = 1, .sem_op = -1, .sem_flg = 0};
    struct sembuf blocked_sem_op = {.sem_num = 0, .sem_op = 0, .sem_flg = 0};
    while (1) {
        semop(sem_id, &wait_full, 1);
        blocked_sem_op.sem_op = -1;
        semop(sem_id, &blocked_sem_op,1);

        memcpy(buff, q->text[q->head],10);
        q->head = (q->head +1)%QUEUE_SIZE;

        blocked_sem_op.sem_op = 1;
        semop(sem_id, &blocked_sem_op, 1);
        semop(sem_id, &post_empty, 1);
        for (int i = 0; i<TEXT_LENGTH; i++) {
            putchar(buff[i]);
            fflush(stdout);
            sleep(1);
        }
        printf("\n");
    }


}
