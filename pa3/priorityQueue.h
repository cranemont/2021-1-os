#include<stdlib.h>

typedef struct Node{
    int timeStamp;
    int lastReferenceTime;
    int referenceCount;
    char pageNum;
    Node* next;
    Node* prev;
} Node;

typedef struct priorityQueueStruct{
    Node* head;
    Node* tail;
    int count;
} priorityQueueStruct;

void QueueInit();

Node* CreateNode(int _timeStamp, char _pageNum);
Node* CreateFIFONode(int _timeStamp, char _pageNum);
Node* CreateLRUNode(int _timeStamp, char _pageNum);
Node* CreateLFUNode(int _timeStamp, char _pageNum);

void PushFIFO(priorityQueueStruct *pq, int _timeStamp, int _pageNum); // timeStamp기준 , timestamp가 가장 작은게 앞에, tail에 삽입
void PushLRU(priorityQueueStruct *pq, int _timeStamp, int _pageNum); // lastReferenceTime기준 , 가장 작은게 앞에, tail에 삽입?
void PushLFU(priorityQueueStruct *pq, int _timeStamp, int _pageNum); // referenceCount기준 , 작은게 앞에 , 같은 값일경우 lastReferenceTIme이 작은게 앞에
Node* searchLFUInsertPos(priorityQueueStruct *pq); // 삽입 불가능 위치 반환. 해당 위치 이전포지션에 삽입

void UpdateLRU(int timeStamp, char _pageNum);
void UpdateLFU(int timeStamp, char _pageNum);
Node* findPage(char _pageNum);

// 앞에있는(front)가 교체될 페이지
void Pop();
void Front();