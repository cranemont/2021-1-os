#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "priorityQueue.h"
#define MAX_STRING_LEN 1000
int* pageFrame; // 페이지 프레임에 존재하는 페이지 저장.
int* pageReferenceBit;

int maxPageNum; // N, 예외 input 처리
int maxPageFrame; // M
int windowSize; // W
int pageReferenceStringLen; // K
int* pageReferenceString;

priorityQueueStruct *pq_FIFO;
priorityQueueStruct *pq_LRU;
priorityQueueStruct *pq_LFU;

void labelPrint(){
    printf("Time\tRef.\tMem.state");
    for(int i=0; i<maxPageFrame-1; i++){
        printf("\t");
    }
    printf("PageFault\n");
}

void printCurrentState(int time, int pageNum, bool pageFault){
    
    printf("%d\t%d\t|", time, pageNum);
    for(int i=0; i<maxPageFrame; i++){
        if(pageFrame[i] != -1){
            printf("%d\t", pageFrame[i]);
        }
        else{
            printf("\t");
        }
    }

    if(pageFault){
        printf("|F\n");
    }
    else{
        printf("|\n");
    }
}

int findLongestPeriod(int time){
    int longestPeriodPage;
    int period = 0;
    for(int i=0; i<maxPageFrame; i++){
        int curPeriod = 1001;
        int page = pageFrame[i];
        for(int j=time; j<pageReferenceStringLen; j++){
            if(pageReferenceString[j] == page){
                curPeriod = j-time+1;
                break;
            }
        }

        if(period < curPeriod){
            longestPeriodPage = page; 
            period = curPeriod;
        }
    }
    return longestPeriodPage;
}

void MIN(){
    int time = 1;
    int pageFrameIndex = 0;
    labelPrint();
    
    while(pageReferenceStringLen >= time){
        int pageNum = pageReferenceString[time - 1];
        bool pageFault = false;
        if(pageReferenceBit[pageNum] == -1){ // page fault
            pageFault = true;
            if(pageFrameIndex == maxPageFrame){ // 자리없음
                int replacePageNum = findLongestPeriod(time);
                int replaceIndex = pageReferenceBit[replacePageNum];
                pageReferenceBit[replacePageNum] = -1;
                pageReferenceBit[pageNum] = replaceIndex;
                pageFrame[replaceIndex] = pageNum;
            }
            else{
                pageFrame[pageFrameIndex] = pageNum;
                pageReferenceBit[pageNum] = pageFrameIndex++;
            }
        }

        printCurrentState(time, pageNum, pageFault);
        ++time;
    }
}

void FIFO(){
    int time = 1;
    int pageFrameIndex = 0;
    labelPrint();
    
    while(pageReferenceStringLen >= time){
        int pageNum = pageReferenceString[time - 1];
        bool pageFault = false;
        if(pageReferenceBit[pageNum] == -1){ // page fault
            pageFault = true;
            if(pageFrameIndex == maxPageFrame){ // 자리없음
                int replacePageNum = Front(pq_FIFO)->pageNum;
                Pop(pq_FIFO);
                int replaceIndex = pageReferenceBit[replacePageNum];
                pageReferenceBit[replacePageNum] = -1;
                pageReferenceBit[pageNum] = replaceIndex;
                pageFrame[replaceIndex] = pageNum;
                PushFIFO(pq_FIFO, time, pageNum);
            }
            else{
                PushFIFO(pq_FIFO, time, pageNum);
                pageFrame[pageFrameIndex] = pageNum;
                pageReferenceBit[pageNum] = pageFrameIndex++;
            }
        }

        printCurrentState(time, pageNum, pageFault);
        ++time;
    }
}

void LRU(){
    int time = 1;
    int pageFrameIndex = 0;
    labelPrint();

    while(pageReferenceStringLen >= time){
        int pageNum = pageReferenceString[time - 1];
        bool pageFault = false;
        if(pageReferenceBit[pageNum] == -1){ // page fault
            pageFault = true;
            if(pageFrameIndex == maxPageFrame){ // 자리없음
                int replacePageNum = Front(pq_LRU)->pageNum;
                Pop(pq_LRU);
                int replaceIndex = pageReferenceBit[replacePageNum];
                pageReferenceBit[replacePageNum] = -1;
                pageReferenceBit[pageNum] = replaceIndex;
                pageFrame[replaceIndex] = pageNum;
                PushLRU(pq_LRU, time, pageNum);
            }
            else{
                PushLRU(pq_LRU, time, pageNum);
                pageFrame[pageFrameIndex] = pageNum;
                pageReferenceBit[pageNum] = pageFrameIndex++;
            }
        }
        else{
            UpdateLRU(pq_LRU, time, pageNum);
        }

        printCurrentState(time, pageNum, pageFault);
        ++time;
    }
}

void LFU(){
    int time = 1;
    int pageFrameIndex = 0;
    labelPrint();

    while(pageReferenceStringLen >= time){
        int pageNum = pageReferenceString[time - 1];
        bool pageFault = false;
        if(pageReferenceBit[pageNum] == -1){ // page fault
            pageFault = true;
            if(pageFrameIndex == maxPageFrame){ // 자리없음
                int replacePageNum = Front(pq_LFU)->pageNum;
                Pop(pq_LFU);
                int replaceIndex = pageReferenceBit[replacePageNum];
                pageReferenceBit[replacePageNum] = -1;
                pageReferenceBit[pageNum] = replaceIndex;
                pageFrame[replaceIndex] = pageNum;
                PushLFU(pq_LFU, time, pageNum);
            }
            else{
                PushLFU(pq_LFU, time, pageNum);
                pageFrame[pageFrameIndex] = pageNum;
                pageReferenceBit[pageNum] = pageFrameIndex++;
            }
        }
        else{
            UpdateLFU(pq_LFU, time, pageNum);
        }

        printCurrentState(time, pageNum, pageFault);
        ++time;
    }
}


void WS(){
    int time = 1;
    int WSPageFrame[100];
    for(int i=0; i<100; i++){
        WSPageFrame[i] = -1;
    }
    int WSPageFrameIndex = 0;

    while(pageReferenceStringLen >= time){
        int stringIndex = time - 1;
        int pageNum = pageReferenceString[stringIndex];
        bool pageFault = false;
        int intervalIndex = stringIndex - windowSize >= 0 ? stringIndex - windowSize : 0;
        if(intervalIndex > 0){
            int intervalIndexPage = pageReferenceString[intervalIndex-1];
            if(pageReferenceBit[intervalIndexPage] != -1){
                //find pageReferenceBit[intervalIndex-1] from page frame and del
                int flag = 0;
                for(int i = intervalIndex; i<=stringIndex; i++){
                    if(pageReferenceString[i] == intervalIndexPage){
                        flag = 1;
                        break;
                    }
                }
                if(!flag){
                    WSPageFrame[pageReferenceBit[intervalIndexPage]] = -1;
                    pageReferenceBit[intervalIndexPage] = -1;
                }
                
            }
        }
        if(pageReferenceBit[pageNum] == -1){ // page fault
            pageFault = true;
            WSPageFrame[pageNum] = pageNum;
            pageReferenceBit[pageNum] = pageNum;
        }

        printf("%d\t%d\t|", time, pageNum);
        for(int i=0; i<10; i++){
            if(WSPageFrame[i] != -1){
                printf("%d\t", WSPageFrame[i]);
            }
            else{
                printf("\t");
            }
        }

        if(pageFault){
            printf("|F\n");
        }
        else{
            printf("|\n");
        }

        ++time;
    }
}

void reset(){
    for(int i=0; i<=maxPageNum; i++){
        pageReferenceBit[i] = -1;
    }
    for(int i=0; i<maxPageFrame; i++){
        pageFrame[i] = -1;
    }
}

int main(int argc, char** argv){
    
    FILE* input = fopen("input.txt", "r");
    fscanf(input, "%d  %d  %d  %d\n", &maxPageNum, &maxPageFrame, &windowSize, &pageReferenceStringLen);
    pageReferenceString = (int*)malloc(sizeof(int)*(pageReferenceStringLen));
    pageFrame = (int*)malloc(sizeof(int)*maxPageFrame);
    pageReferenceBit = (int*)malloc(sizeof(int)*(maxPageNum+1));

    for(int i=0; i<pageReferenceStringLen; i++){
        fscanf(input, "%d", &pageReferenceString[i]);
    }
    
    pq_FIFO = (priorityQueueStruct*)malloc(sizeof(priorityQueueStruct));
    pq_LRU = (priorityQueueStruct*)malloc(sizeof(priorityQueueStruct));
    pq_LFU = (priorityQueueStruct*)malloc(sizeof(priorityQueueStruct));

    QueueInit(pq_FIFO);
    QueueInit(pq_LRU);
    QueueInit(pq_LFU);
    
    reset();
    printf("***********[ MIN ]***********\n");
    MIN();
    printf("******************************\n\n");

    reset();
    printf("***********[ FIFO ]***********\n");
    FIFO();
    printf("******************************\n\n");

    reset();
    printf("***********[ LRU ]***********\n");
    LRU();
    printf("******************************\n\n");

    reset();
    printf("***********[ LFU ]***********\n");
    LFU();
    printf("******************************\n\n");

    reset();
    printf("***********[ WS ]***********\n");
    WS();
    printf("******************************\n\n");
    return 0;
}