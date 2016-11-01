//
//  function.h
//  ConsumerProducer
//
//  Created by артем on 01.11.16.
//  Copyright © 2016 артем. All rights reserved.
//
#include "String"
#include "Vector"
#include "iostream"
using namespace std;
#ifndef function_h
#define function_h


int* PrepareData(int* array,int number) {
    for (int i=0; i< number; i++) {
        *(array+i)=1;
    }
    return array;
}

void initArguments(int start,int maxIndex,vector<int> value) {
    start = 0;
    maxIndex = (int)value.size();
}

int* ChangeData(int* value,int number) {
    for (int i = 0; i< number; i++) {
        if ( *(value+i) == 9) {
            *(value+i) = 0;
        } else {
            ( *(value+i) )++;
        }
    }
    return value;
}

void UseData(int* value,int number) {
    for (int i =0; i< number; i++) {
        cout << *(value+i) <<" ";
    }
    cout << endl;
}

void Producer (int* value,int number ) {
    cout << "Producer" << endl
         <<"start value = ";
    UseData(value,number);
    value=ChangeData(value,number);
    cout <<"Finish value = ";
    UseData(value,number);
}

void Consumer (int*value,int number) {
    cout << "Consumer" << endl;
    UseData(value,number);
}

#endif /* function_h */
