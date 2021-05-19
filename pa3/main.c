#include <stdio.h>
#include <stdlib.h>

typedef struct heapNode{
    int timeStamp;
    int lastReferenceTime;
    int referenceCount;
    char pageNum;
} heapNode;

int time;

void HeapInsertFIFO();
void HeapInsertLRU();
void HeapInsertLFU();

void HeapPop();
void HeapTop();


int main(int argc, char** argv){
    time = 0;

}