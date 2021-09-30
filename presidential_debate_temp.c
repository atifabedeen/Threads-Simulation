#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

static sem_t connected_lock;
int next_id = 1;
static sem_t id_lock;
static sem_t operators;
static int NUM_OPERATORS = 2;
static int NUM_LINES = 5;
static int connected = 0;

void* phonecall(void* vargp) {
    int id = 0;
    sem_wait(&id_lock);
    id = next_id++;
    sem_post(&id_lock);
    printf("This is phonecall %i\n",id);
    pthread_detach(pthread_self());
    //Thread
}

int main(int argc, char* argv[]) {
    sem_init(&id_lock,0,1);
    pthread_t phonecall_id[200];
    for(int i = 1; i <= 10; ++i) {
        pthread_create(&phonecall_id[i], NULL, phonecall, NULL);
    }
}

