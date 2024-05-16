#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define MaxCrops 5
#define warehouseSize 5

sem_t empty; 

sem_t full; 
int in = 0; 
int out = 0; 

char crops[5] = {'R', 'W', 'P', 'S', 'M'}; 
char warehouse[5] = {'N', 'N', 'N', 'N', 'N'}; 
pthread_mutex_t mutex; 

void *Farmer(void *far)
{
    int *farNo = (int *)far; 

    for (int i = 0; i < MaxCrops; i++) 
    {
        sem_wait(&empty); 
        
        pthread_mutex_lock(&mutex); 
        
        warehouse[in] = crops[i]; 

        printf("Farmer %d: Insert crops %c at %d\n", *farNo, crops[i], in);

        in = (in + 1) % warehouseSize; 
        pthread_mutex_unlock(&mutex); 

        sem_post(&full); 
    }

    printf("Farmer%d: ", *farNo + 1); 
    
    for (int j = 0; j < warehouseSize; j++)
    {
        printf("%c", warehouse[j]);
    }
    printf("\n");
    pthread_exit(NULL);
}

void *ShopOwner(void *sho)
{
    int *shoNo = (int *)sho; 

    for (int i = 0; i < MaxCrops; i++) 
    {
        sem_wait(&full); 

        pthread_mutex_lock(&mutex); 

        char crop = warehouse[out]; 

        warehouse[out] = 'N'; 
        
        printf("Shop owner %d: Remove crops %c from %d\n", *shoNo, crop, out);
        
        out = (out + 1) % warehouseSize; 
        
        pthread_mutex_unlock(&mutex);
        sem_post(&empty); 
    }
    
    printf("ShopOwner%d: ", *shoNo + 1);
    for (int j = 0; j < warehouseSize; j++) 
    {
        printf("%c", warehouse[j]);
    }
    printf("\n");

    pthread_exit(NULL);
}

int main()
{
    pthread_t farmers[5], shopOwners[5]; 
    pthread_mutex_init(&mutex, NULL); 
    sem_init(&empty, 0, warehouseSize);
    sem_init(&full, 0, 0); 
    int a[5] = {1, 2, 3, 4, 5}; 

    for (int i = 0; i < 5; i++)
    {
        pthread_create(&farmers[i], NULL, Farmer, &a[i]);
        pthread_create(&shopOwners[i], NULL, ShopOwner, &a[i]);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(farmers[i], NULL);
        pthread_join(shopOwners[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty); // 
    sem_destroy(&full);

    return 0;
}

