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


int main() {
    Queue *q;
    key_t shm_key = ftok("/tmp", 'A');
    int shm_id = shmget(shm_key, sizeof(Queue), 0666);
    q = shmat(shm_id, NULL, 0);

    key_t sem_key = ftok("/tmp", 'B');
    int sem_id = semget(sem_key, 3, 0666);

    struct sembuf acquire = {0,-1,0};
    struct sembuf release = {0,1,0};
    struct sembuf wait_empty = {2, -1,0};
    struct sembuf post_full = {1, 1,0};
    char buff[TEXT_LENGTH];
    srand(time(NULL));
    while (1) {
        for (int i = 0; i < TEXT_LENGTH; i++) {
            buff[i] = 'a' + rand()%26;
            putchar(buff[i]);
            fflush(stdout);
        }
        semop(sem_id, &wait_empty, 1);
        semop(sem_id, &acquire,1);

        memcpy(q->text[q->tail], buff, TEXT_LENGTH);
        q->tail  = (q->tail +1) % QUEUE_SIZE;

        semop(sem_id, &release,1);
        semop(sem_id, &post_full,1);
        printf("Dodano do kolejki\n");
        sleep(rand()%10 +1);


    }
}