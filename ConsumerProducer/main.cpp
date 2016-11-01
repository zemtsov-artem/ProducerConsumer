//
//  main.cpp
//  ConsumerProducer
//
//  Created by артем on 01.11.16.
//  Copyright © 2016 артем. All rights reserved.
//
#include "function.h"
#include "pthread.h"
#include "mpi.h"




int main(int argc, char * argv[]) {
    int numproc,procId;
    int Amount = 10;
    int* mainData;
    int array[5]={0,0,0,0,0};//проверка на простом массиве измененном в 0 проц
    mainData=PrepareData(mainData,Amount);
    pthread_mutex_t mutex;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    
    
    
    if (procId == 0) {
        pthread_mutex_init(&mutex, PTHREAD_MUTEX_NORMAL);
        pthread_mutex_lock(&mutex);
        UseData(mainData, Amount);
        for (int i=0; i<5; i++) {
            array[i]=i;
        }
        MPI_Bcast(mainData, Amount, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(array, 5, MPI_INT, 0, MPI_COMM_WORLD);
        pthread_mutex_unlock(&mutex);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    if ( (procId%2) == 0){
        // Producer
            pthread_mutex_lock(&mutex);
            Producer(mainData,Amount);
            MPI_Bcast(mainData, Amount, MPI_INT, procId, MPI_COMM_WORLD);
        
            pthread_mutex_unlock(&mutex);
    }
    else{
        // Consumer
            pthread_mutex_lock(&mutex);
            Consumer(mainData,Amount);
            MPI_Bcast(mainData, Amount, MPI_INT, procId, MPI_COMM_WORLD);
            pthread_mutex_unlock(&mutex);
        
            for (int i=0; i<5; i++) {
                cout<<array[i];
            }
            cout<<endl<<endl;
        
        
    }
    

    
    MPI_Barrier(MPI_COMM_WORLD);
    if(procId==0) {
        pthread_mutex_destroy(&mutex);
    }
    MPI_Finalize();
    return 0;
}
