#include <stdio.h>
#include <stdlib.h>
#include <priorityQueue.h>

int time;

char* pageFrame; // 페이지 프레임에 존재하는 페이지 저장.
int pageFrameIndex;

int maxPageNum; // N, 예외 input 처리
int maxPageFrame; // M
int windowSize; // W
int pageReferenceStringLen; // K
char* pageReferenceString;

priorityQueueStruct pq_FIFO;
priorityQueueStruct pq_LRU;
priorityQueueStruct pq_LFU;

queueInit(pq_FIFO);
queueInit(pq_LRU);
queueInit(pq_LFU);


int main(int argc, char** argv){
    pageReferenceString = (char*)malloc(sizeof(char)*(pageReferenceStringLen+1));
    pageFrame = (char*)malloc(sizeof(char)*maxPageFrame);
    
    FILE* input = fopen("input.txt", 'r');
    fscanf("%d %d %d %d\n", &maxPageNum, &maxPageFrame, &windowSize, &pageReferenceStringLen);
    fscanf("%s", pageReferenceString);
}