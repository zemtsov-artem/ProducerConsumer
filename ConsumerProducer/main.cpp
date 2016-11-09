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

static int* mainData=new int[10];


int main(int argc, char * argv[]) {
    int numproc,procId;
    int Amount = 10;
    int ProducerFlag = 0,ConsumerFlag = 0;
    
    mainData=PrepareData(mainData,Amount);
    pthread_mutex_t mutex;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    
    if (procId == 0) {
        //pthread_mutex_init(&mutex, PTHREAD_MUTEX_NORMAL);
        //pthread_mutex_lock(&mutex);
        UseData(mainData, Amount);
        //pthread_mutex_unlock(&mutex);
    }
    MPI_Bcast(mainData, Amount, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    
    if ( (procId%2) == 0){
        // Producer
            //pthread_mutex_lock(&mutex);
            Producer(mainData,Amount);
            ProducerFlag=procId;
            //pthread_mutex_unlock(&mutex);
            UseData(mainData, Amount);
    }
    
    //MPI_Bcast(mainData, Amount, MPI_INT, ProducerFlag, MPI_COMM_WORLD);
    
    
    if (procId%2==0) {
        MPI_Bcast(mainData, Amount, MPI_INT, ProducerFlag, MPI_COMM_WORLD);
        //MPI_Bcast(mainData, Amount, MPI_INT, procId, MPI_COMM_WORLD);
    }

    if( (procId%2) != 0){
        // Consumer
            pthread_mutex_lock(&mutex);
            Consumer(mainData,Amount);
            ConsumerFlag=procId;
            pthread_mutex_unlock(&mutex);
    }
    MPI_Bcast(mainData, Amount, MPI_INT, ConsumerFlag, MPI_COMM_WORLD);
    

    
    MPI_Barrier(MPI_COMM_WORLD);
    //if(procId==0) {
      //  pthread_mutex_destroy(&mutex);
    //}
   
    MPI_Finalize();
   
    delete[] mainData;
    return 0;
}
