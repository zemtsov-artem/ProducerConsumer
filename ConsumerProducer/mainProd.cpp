//
//  mainProd.cpp
//  ConsumerProducer
//
//  Created by артем on 15.11.16.
//  Copyright © 2016 артем. All rights reserved.
//

#include <mpi.h>
#include <iostream>
#include <queue>
#include "time.h"

#define MANAGER 0

struct info {
    int rank;
    int whatDoYouNeed;
    int res;
};

const int PUT_RESOURCE = 1;
const int GET_RESOURCE = 2;
const int EXITP = 3;
const int EXITC = 4;
int STOP = -1;



class Consumer { //потребитель
    
private:
    int resources_to_consume; //ресурсы потребителя
    std::queue<int> resources; //очередь ресурсов
    info i1;
    MPI_Status status;
    void RequestResource() { //запрос ресурсов
        MPI_Send(&i1, 3, MPI_INT, MANAGER, 0, MPI_COMM_WORLD); //передача сообщения менеджеру от определенного потербителя //my
    }
    void RecieveResource() { //получение ресурсов
        int resource = 0;
        MPI_Status status;
        MPI_Recv(&resource, 1, MPI_INT, MANAGER, 3, MPI_COMM_WORLD, &status); //получение ресурсов от менеджера
        if (resource == -1){
            MPI_Finalize();
            exit(0);
        }
        if (resource != 0){
            resources.push(resource); //добавление ресурсов в очередь
            std::cout<<"Consumer "<< i1.rank<< " : i`ve got resource " << resources.back()<<std::endl;
            resources_to_consume--; //уменьшение кол-ва ресурсов на единицу
        }
    }
    
public:
    Consumer(int in_rank, int in_resource_num) { //конструктор
        resources_to_consume = in_resource_num; //задаем кол-во ресурсов для потребителя
        i1.rank = in_rank; //задаем процесс для потребителя
        i1.whatDoYouNeed = GET_RESOURCE;
    }
    void Run() { //запуск
        while (resources_to_consume) { //пока не кончились ресурсы
            RequestResource(); //запрашивать и получать ресурсы
            RecieveResource();
        }
        i1.whatDoYouNeed = EXITC;
        MPI_Send(&i1, 3, MPI_INT, MANAGER, 0, MPI_COMM_WORLD);//my
        
    }
};

class Producer { //производитель
private:
    int resources_to_produce; //ресурсы производителя
    std::queue<int> resources; //очередь ресурсов
    MPI_Status status;
    info i2;
    int k;

public:
    Producer(int in_rank, int amount) { //конструктор
        i2.rank = in_rank; //задаем ранг
        resources_to_produce = amount;  //задаем кол-во ресурсов
        i2.whatDoYouNeed = PUT_RESOURCE;
        k = resources_to_produce;
    }
    void Run()
    { //запуск
        while (resources_to_produce) { //пока есть ресурсы
            
            CreateResource(); //создаем ресурс
            SendResourceToManager(); //отправляем ресурс в менеджер
        }
        while (!resources.empty()) { //пока очередь ресурсов не закончится
            SendResourceToManager(); //отправляем ресурс потребителю
        }
        i2.whatDoYouNeed = EXITP;
        MPI_Send(&i2, 3, MPI_INT, MANAGER, 0, MPI_COMM_WORLD);//my
    }
    
private:
    void CreateResource() { //создаем ресурс
        //std::cout<<"Check trash - "<<k-resources_to_produce<<std::endl;
        int resource;
        int coefficient=k-resources_to_produce+1;
        if (i2.rank==1){
            resource =(i2.rank*100)+coefficient;
        }
        else {
            resource =((i2.rank-i2.rank/2) *100)+coefficient;
        }
        resources.push(resource); //добавляем ресурсы в очередь
        resources_to_produce--; //уменьшаем кол-во ресурсов производителя на 1
    }

    void SendResourceToManager() { //отправление ресурса в буфер
        //std::cout<<"First resourse is - "<<resources.front()<< std::endl;
        i2.res = resources.front();
        int answear;
        
        MPI_Send(&i2, 3, MPI_INT, MANAGER, 0, MPI_COMM_WORLD);//my
        MPI_Recv(&answear, 1, MPI_INT, MANAGER, 1, MPI_COMM_WORLD, &status); //получаем ответ с информацией о соcтоянии буфера
        
        if (answear == -1) {
            MPI_Finalize();
            exit(0);
        }
        if (answear == 0){ //если буфер не полон
            std::cout<< "Producer "<<i2.rank<<": sending resource in buffer"<<std::endl;
            resources.pop(); //удаляем ресурс из очереди
        }else{
            std::cout << "Producer "<<i2.rank<<": buffer is full. Failed to put the resource in buffer"<<std::endl;
        }
    }
};

class Manager { //менеджер
private:
    int total_resources; //общее кол-во ресурсов
    
    int *buffer;
    int N,p_size,pro,con;
    MPI_Status status;
    info i3;

public:
    Manager(int in_total_resources, int proc_size,int p,int c) { // конструктор
        total_resources = in_total_resources;
        p_size = proc_size;
        N = total_resources;
        con = c;
        pro = p;
        buffer = new int[total_resources];
        for (int i = 0; i < total_resources; i++)
        {
            buffer[i] = 0;
        }
    }
    
    void Run() { //запуск
        while (true) {
            MPI_Recv(&i3, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            //std::cout<<"Check "<<i3.res<<std::endl;
            if (i3.whatDoYouNeed == EXITP){
                pro--;
            }
            if (i3.whatDoYouNeed == EXITC){
                con--;
                if (con == 0){
                    for (int i = 1; i < p_size; i++){
                        if (i%2){
                            MPI_Send(&STOP, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                        }else {
                            MPI_Send(&STOP, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
                        }
                    }
                    MPI_Finalize();
                    exit(0);
                }
            }
            
            if (i3.whatDoYouNeed == PUT_RESOURCE){
                Put(i3.rank, i3.res);
            }
            if (i3.whatDoYouNeed == GET_RESOURCE){
                Get(i3.rank);
            }
        }
    }
    
private:
    void Put(int producer_id, int resource) { //кладем элемент в буфер
        int answear = 1;
        for (int i = 0; i < N; i++) {
            if (buffer[i] == 0) {
                answear = 0;
                buffer[i] = resource;
                break;
            }
            else if(i==N-1){
                std::cout<<"Buffer is full"<<std::endl;
            }
        }
        if (answear == 0) {
            std::cout<< "Manager: producer "<<producer_id<< " put resource "<<resource<<std::endl;
        }
        MPI_Send(&answear, 1, MPI_INT, producer_id, 1, MPI_COMM_WORLD); //отправляем ответ с информацией, полон буфер или нет
    }
    
    void Get(int consumer_id) { //забираем элемент из буфера
        int resource=0;
        for (int i = 0; i < N; i++){
            if (buffer[i] == 0)
                resource = 0;
            else {
                resource = buffer[i];
                buffer[i] = 0;
                break;
            }
        }
        if (resource != 0) {
            std::cout<<"Manager: consumer "<<consumer_id<<" get resource "<< resource << std::endl;
        }else {
            if (pro == 0){
                for (int i = 1; i < p_size; i++){
                    std::cout << "p_size1= " << i<<std::endl;
                    if (i % 2)
                        MPI_Send(&STOP, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                    else
                        MPI_Send(&STOP, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
                }
            }
        }
        MPI_Send(&resource, 1, MPI_INT, consumer_id, 3, MPI_COMM_WORLD);
    }

};

int main(int argc, char** argv) {
    
    MPI_Init(&argc, &argv);
    
    int rank = -1;
    int process_num = -1;
    int pro, con;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //определяем ранг процесса
    MPI_Comm_size(MPI_COMM_WORLD, &process_num); //определяем кол-во процессов
    
    int a = (process_num - 1);
    pro = a / 2;
    con = a / 2;
    if (a % 2)
        pro ++;
    
    if (rank == 0) { //если в нулевом процессе
        Manager manager(3,process_num,pro,con); 
        manager.Run(); //запускаем менеджера
    }
    else { //если не в нулевом процессе
        if (rank % 2) { //каждый второй процесс будет производителем
            std::cout << "The process with the rank of " << rank << " is Producer" << std::endl;
            Producer producer(rank,5); //создаем производителя для определенного процесса с 5 ресурсами
            producer.Run(); //запускаем его
        }
        else { //а остальные потребителями
            std::cout << "The process with the rank of " << rank << " is Consumer" << std::endl;
            Consumer consumer(rank,5); //создаем потребителя для определенного процесса с 5 ресурсами
            consumer.Run(); //создаем потребителя
        }
    }
    
    MPI_Finalize(); //заканчивем работу MPI
    return 0;
}
