#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 3


pthread_mutex_t mutex;
int sum;
int status = 0;
int caller = 0;


pthread_t    tid[5];
pthread_mutex_t    mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t    cond = PTHREAD_COND_INITIALIZER;


void* child_thread(void* arg) {
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mutex);
        int random = rand() % 100;
        if (random < 10) {
            printf("\n%d child thread : message send\n", *((int*)arg));
            status = 1;
            caller = *((int*)arg);
        }
        pthread_cond_wait(&cond, &mutex);
        printf("%d chile thread : recieve from parent\n", *((int*)arg));

        pthread_mutex_unlock(&mutex);
    }
}

void* parent_thread(void* arg) {
    while (1) {
        sleep(1);
        if (status == 1) {
            pthread_mutex_lock(&mutex);
            pthread_cond_broadcast(&cond);
            printf("\nparent thread : message send\n\n");
            status = 0;
            pthread_mutex_unlock(&mutex);
        }
    }
}

void main() {
    srand(time(NULL));
    int i;
    int id[5];

    pthread_create(&tid[0], NULL, parent_thread, (void*)&id[0]);
    for (i = 1; i < 5; i++) {
        id[i] = i;
        pthread_create(&tid[i], NULL, child_thread, (void*)&id[i]);
    }

    for (i = 1; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
