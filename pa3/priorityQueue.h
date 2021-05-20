

typedef struct Node{
    int timeStamp;
    int lastReferenceTime;
    int referenceCount;
    int pageNum;
    int cc;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct priorityQueueStruct{
    Node* head;
    Node* tail;
    int count;
} priorityQueueStruct;

void QueueInit(priorityQueueStruct *pq);

Node* CreateNode(int _timeStamp, int _pageNum);
Node* CreateFIFONode(int _timeStamp, int _pageNum);
Node* CreateLRUNode(int _timeStamp, int _pageNum);
Node* CreateLFUNode(int _timeStamp, int _pageNum);

void PushFIFO(priorityQueueStruct *pq, int _timeStamp, int _pageNum); // timeStamp기준 , timestamp가 가장 작은게 앞에, tail에 삽입
void PushLRU(priorityQueueStruct *pq, int _timeStamp, int _pageNum); // lastReferenceTime기준 , 가장 작은게 앞에, tail에 삽입?
void PushLFU(priorityQueueStruct *pq, int _timeStamp, int _pageNum); // referenceCount기준 , 작은게 앞에 , 같은 값일경우 lastReferenceTIme이 작은게 앞에
Node* searchLFUInsertPos(priorityQueueStruct *pq, int _referenceCount); // 삽입 불가능 위치 반환. 해당 위치 이전포지션에 삽입

void UpdateLRU(priorityQueueStruct *pq, int _timeStamp, int _pageNum);
void UpdateLFU(priorityQueueStruct *pq, int _timeStamp, int _pageNum);
Node* findPage(priorityQueueStruct *pq, int _pageNum);

// 앞에있는(front)가 교체될 페이지
void Pop(priorityQueueStruct *pq);
Node* Front(priorityQueueStruct *pq);