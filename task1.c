#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX 10
#define BUFLEN 6
#define NUMTHREAD 2

void *consumer(void *id);
void *producer(void *id);

char buffer[BUFLEN]; 
char source[BUFLEN]; 
int pCount = 0; 
int cCount = 0; 
int buflen;

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER; 

pthread_cond_t nonEmpty = PTHREAD_COND_INITIALIZER;      

pthread_cond_t full = PTHREAD_COND_INITIALIZER;        

int thread_id[NUMTHREAD] = {0, 1};
int i = 0;
int j = 0;

int main()
{
    int i;
    pthread_t thread[NUMTHREAD];

    strcpy(source, "abcdef");
    buflen = strlen(source);

    pthread_create(&thread[0], NULL, producer, (void *)&thread_id[0]);
    pthread_create(&thread[1], NULL, consumer, (void *)&thread_id[1]);

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);

    return 0;
}

void *producer(void *arg)
{
    int *id = (int *)arg;
    
    for (int i = 0; i < MAX; i++)
    {
        pthread_mutex_lock(&count_mutex); 
        
        while (pCount == BUFLEN) 
        {
            pthread_cond_wait(&full, &count_mutex); 
        }
        buffer[pCount] = source[i % BUFLEN]; 
        
        printf("%d produced %c by Thread %d\n", i, buffer[pCount], *id);
        pCount++;
        
        pthread_cond_signal(&nonEmpty); 
        
        pthread_mutex_unlock(&count_mutex); 
        
        sleep(1); 
    }
    return NULL;
}

void *consumer(void *arg)
{
    int *id = (int *)arg; 
   
    for (int i = 0; i < MAX; i++) 
    {
        pthread_mutex_lock(&count_mutex); 
       
       while (pCount == 0)  
        {
            pthread_cond_wait(&nonEmpty, &count_mutex);
        }

        printf("%d consumed %c by Thread %d\n", i, buffer[0], *id); 
        
        for (int j = 0; j < pCount - 1; j++)
        {
            buffer[j] = buffer[j + 1]; 
        }
        pCount--; 
        pthread_cond_signal(&full); 
        pthread_mutex_unlock(&count_mutex); 
        sleep(1); 
    }
    return NULL;
}

