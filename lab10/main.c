#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int waiting_patients_n = 0;
int waiting_patients[3];
int medicine_n = 6;
int pharmacist_waiting=0;
int global;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t doctor_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t patient_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pharmacist_waiting_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pharmacist_delivering_cond = PTHREAD_COND_INITIALIZER;

int random_range(int min_val, int max_val) {
    return min_val + rand() % (max_val - min_val + 1);
}

void printer(int id, char* role,char* msg) {
    time_t now;
    struct tm *infotime;
    char buff[80];
    time(&now);
    infotime = localtime(&now);
    strftime(buff, sizeof(buff), "%H:%M:%S", infotime);
    printf("[%s] - %s(%d): %s\n", buff, role, id, msg );
}

void* doctor(void* args) {
    char msg[100];
    int id = 0;
    char *role = "Lekarz";
    while (global) {
        pthread_mutex_lock(&mutex);
        while (!((waiting_patients_n ==3 && medicine_n >=3) ||
            (pharmacist_waiting ==1&& medicine_n <3))) {
            pthread_cond_wait(&doctor_cond, &mutex);
        }
        if (waiting_patients_n ==3 && medicine_n >=3) {
            sprintf(msg, "konsultuję pacjentów %d, %d, %d", waiting_patients[0], waiting_patients[1], waiting_patients[2]);
            printer(id, role, msg);
            medicine_n -=3;
            waiting_patients_n =0;
            sleep(random_range(2,4));
            pthread_cond_broadcast(&patient_cond);
            pthread_cond_signal(&pharmacist_waiting_cond);
            global -=3;
        }
        else if (pharmacist_waiting ==1&& medicine_n !=6) {
            printer(id, role, "przyjmuję dostawę leków");
            medicine_n=6;
            pharmacist_waiting=0;
            sleep(random_range(1,3));

            pthread_cond_broadcast(&pharmacist_delivering_cond);
            global-=1;
        }
        printer(id, role, "zasypiam");
        pthread_mutex_unlock(&mutex);
    }
    return  NULL;

}
void* patient(void* args) {
    int id = *(int*)args;
    free(args);
    char msg[100];
    char *role = "Patcjent";
    int time = random_range(2,5);
    sprintf(msg, "Ide do szpitala, bede za %d s", time);
    printer(id, role, msg);
    sleep(time);
    pthread_mutex_lock(&mutex);
    while (waiting_patients_n>=3) {
        pthread_mutex_unlock(&mutex);
        time = random_range(2,5);
        sprintf(msg, "za dużo pacjentów, wracam później za %d s)", time);
        printer(id, role, msg);
        sleep(time);
        pthread_mutex_lock(&mutex);
    }
    waiting_patients[waiting_patients_n++] = id;
    sprintf(msg, "czeka %d pacjentów na lekarza", waiting_patients_n);
    printer(id, role, msg);
    if (waiting_patients_n ==3) {
        pthread_cond_signal(&doctor_cond);
        printer(id, role, "budzi lekarza");
    }
    pthread_cond_wait(&patient_cond, &mutex);
    printer(id, role, "kończy wizytę");
    pthread_mutex_unlock(&mutex);
    return NULL;

}
void* pharmacist(void* args) {
    int id = *(int*)args;
    free(args);
    char msg[100];
    char *role = "Farmaceuta";
    int time = random_range(5,15);
    sprintf(msg, "Ide do szpitala, bede za %d s", time);
    printer(id, role, msg);
    sleep(time);
    pthread_mutex_lock(&mutex);
    while (medicine_n == 6 || pharmacist_waiting ==1) {
        pthread_cond_wait(&pharmacist_waiting_cond, &mutex);
    }
    if (medicine_n <3) {
        printer(id, role, "budzę lekarza");
        pharmacist_waiting = 1;
        pthread_cond_signal(&doctor_cond);
        printer(id, role, "dostarczam leki");
        pthread_cond_wait(&pharmacist_delivering_cond, &mutex);
        printer(id, role, "kończę wizytę");
    }
    pthread_mutex_unlock(&mutex);
    return  NULL;

}
int main(int argc, char *argv[]){
    srand(time(NULL));
    int n_patients = atoi(argv[1]);
    int n_pharmacists = atoi(argv[2]);
    global = 20;

    pthread_t *patients_pthread = malloc(n_patients * sizeof(pthread_t));
    pthread_t *pharmacists_pthread = malloc(n_pharmacists * sizeof(pthread_t));
    pthread_t doctor_pthread;

    pthread_create(&doctor_pthread, NULL, doctor,NULL );
    for (int i=0; i< n_patients; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&patients_pthread[i], NULL, patient, id);
    }
    for (int i=0; i< n_pharmacists; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&pharmacists_pthread[i], NULL, pharmacist, id);
    }
    for (int i=0; i< n_patients; i++) {
        pthread_join(patients_pthread[i], NULL);
    }
    for (int i=0; i< n_pharmacists; i++) {
        pthread_join(pharmacists_pthread[i], NULL);
    }
    free(patients_pthread);
    free(pharmacists_pthread);
}