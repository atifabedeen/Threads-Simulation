#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

static sem_t connected_lock; //binary semaphore used to protect the updation of the connected variable
static int next_id = 0; //static variable which counts the number of threads being created
sem_t id_lock; //binary semaphore used to protect the updatio of the next_id variable
static sem_t operators; //counting semaphore used to protect the region of code where the caller is speaking to the operator
static int NUM_OPERATORS = 2; //Maximum number of operators available at a given time
static int NUM_LINES = 5; //Maximum number of call lines available at a given time
static int connected = 0; //static variable used to count the number of available call lines at a given time

/* Thread handler function for the callers
 * paramter: vargp - not used.
 */

void* phonecall(void* vargp) {
    int id = 0; // variable used to give a unique ID to each thread
    sem_wait(&id_lock);
    next_id++;
    sem_post(&id_lock);
    id = next_id;
    printf("Thread [%i] is attempting to connect... \n",id);
    while(true) {
        sem_wait(&connected_lock);
        if(connected < NUM_LINES) {
            connected++;
            sem_post(&connected_lock);
            printf("Thread [%i] connects to an available line, call ringing ...\n", id);
            sem_wait(&operators);
            printf("Thread [%i] is speaking to an operator. \n", id);
            sleep(1);
            printf("Thread [%i] has proposed a question for candidates! The operator has left ... \n", id);
            sem_post(&operators);
            printf("Thread [%i] has hung up! \n", id);
            sem_wait(&connected_lock);
            connected--;
            sem_post(&connected_lock);
            pthread_detach(pthread_self());
            return NULL;
        }
        else if(connected == NUM_LINES) {
            sem_post(&connected_lock);
            sleep(1);
            continue;
        }
    }
}

/* Timer thread handler function.
 * Parameter: vargp - integer pointer to an adress of a variable which stores the number of seconds.
 */

void* timerThread(void* vargp) {
    int timer = *((int*) vargp); //timer variable
    sleep(timer);
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        printf("Too few or too many arguments");
        exit(1);
    }

    int seconds = atoi(argv[1]); //stores the number of seconds
    sem_init(&id_lock,0,1); //initialise the binary semaphore for caller ID
    sem_init(&connected_lock,0,1); //initialise the binary semaphore for call lines
    sem_init(&operators,0,NUM_OPERATORS); //initialise the counting semaphore for operator
    pthread_t phonecall_id[200];
    pthread_t timer_id;
    pthread_create(&timer_id, NULL, timerThread, &seconds);

    for(int i = 1; i <= 200; ++i) {
        pthread_create(&phonecall_id[i], NULL, phonecall, NULL);
    }
    pthread_join(timer_id, NULL);
    sem_destroy(&id_lock);
    sem_destroy(&connected_lock);
    sem_destroy(&operators);
    exit(0);
}
