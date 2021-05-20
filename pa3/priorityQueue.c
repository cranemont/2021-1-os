#include "priorityQueue.h"
#include<stdio.h> //없애기
#include<stdlib.h>
#define DEFAULT 1
void QueueInit(priorityQueueStruct *pq){
    pq->head = (Node*)malloc(sizeof(Node));
    pq->tail = (Node*)malloc(sizeof(Node));
    pq->head->next = pq->tail;
    pq->head->prev = NULL;
    pq->tail->next = NULL;
    pq->tail->prev = pq->head;
    pq->count = 0;
}

/* CREATE NODE */

Node* CreateNode(int _timeStamp, int _pageNum){
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->timeStamp = _timeStamp;
    newNode->pageNum = _pageNum;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

Node* CreateFIFONode(int _timeStamp, int _pageNum){
    Node* newNode = CreateNode(_timeStamp, _pageNum);
    return newNode;
}

Node* CreateLRUNode(int _timeStamp, int _pageNum){
    Node* newNode = CreateNode(_timeStamp, _pageNum);
    newNode->lastReferenceTime = _timeStamp;
    return newNode;
}

Node* CreateLFUNode(int _timeStamp, int _pageNum){
    Node* newNode = CreateNode(_timeStamp, _pageNum);
    newNode->lastReferenceTime = _timeStamp;
    newNode->referenceCount = 1;
    newNode->cc = DEFAULT;
    return newNode;
}


/* PUSH */

void _insert(Node* targetPos, Node* newNode){
    newNode->next = targetPos;
    newNode->prev = targetPos->prev;
    targetPos->prev->next = newNode;
    targetPos->prev = newNode;
}

void PushFIFO(priorityQueueStruct *pq, int _timeStamp, int _pageNum){
    Node* newNode = CreateFIFONode(_timeStamp, _pageNum);
    _insert(pq->tail, newNode);
    ++pq->count;
}
void PushLRU(priorityQueueStruct *pq, int _timeStamp, int _pageNum){
    Node* newNode = CreateLRUNode(_timeStamp, _pageNum);
    _insert(pq->tail, newNode);
    ++pq->count;
}
void PushLFU(priorityQueueStruct *pq, int _timeStamp, int _pageNum){
    Node* newNode = CreateLFUNode(_timeStamp, _pageNum);
    Node* targetPos = searchLFUInsertPos(pq, 1);
    _insert(targetPos, newNode);
    ++pq->count;
}

Node* searchLFUInsertPos(priorityQueueStruct *pq, int _referenceCount){
    if(pq->count == 0){
        return pq->tail;
    }

    Node* pos = pq->head->next;
    while(pos != pq->tail){
        if(pos->referenceCount > _referenceCount){
            return pos;
        }
        pos = pos->next;
    }
    return pos;
}

Node* findPage(priorityQueueStruct *pq, int _pageNum){
    Node* target = pq->head->next;
    while(target != pq->tail){
        if(target->pageNum == _pageNum){
            return target;
        }
        target = target->next;
    }
    return NULL;
}

void UpdateLRU(priorityQueueStruct *pq, int _timeStamp, int _pageNum){
    Node* targetPageFrame = findPage(pq, _pageNum);
    Node* tmp = pq->head->next;
    targetPageFrame->lastReferenceTime = _timeStamp;
    targetPageFrame->prev->next = targetPageFrame->next;
    targetPageFrame->next->prev = targetPageFrame->prev;
    _insert(pq->tail, targetPageFrame);
}

void UpdateLFU(priorityQueueStruct *pq, int _timeStamp, int _pageNum){
    
    Node* targetPageFrame = findPage(pq, _pageNum);
    targetPageFrame->lastReferenceTime = _timeStamp;
    targetPageFrame->referenceCount += 1;
    targetPageFrame->prev->next = targetPageFrame->next;
    targetPageFrame->next->prev = targetPageFrame->prev;
    Node* targetPos = searchLFUInsertPos(pq, targetPageFrame->referenceCount);
    _insert(targetPos, targetPageFrame);
}

void Pop(priorityQueueStruct *pq){
    Node* front = Front(pq);
    pq->head->next = front->next;
    front->next->prev = pq->head;
    free(front);
}

Node* Front(priorityQueueStruct *pq){
    return pq->head->next;
}
