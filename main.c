#include<stdio.h>
#include<stdlib.h>

typedef struct ProcessStruct {
    int pid;
    int arrival_time;
    int queue_pos;
    int cycles;
    int *cpu_burst_arr;
    int *io_burst_arr;
    int wt;
    int tt;
} Process;

typedef struct QNodeStruct {
    Process *p;
    struct QNode *next;
} QNode;

typedef struct QueueStruct {
    QNode *front;
    QNode *rear;
    int count;
    int time_quantum; // for RR
} Queue;

typedef struct ReadyQueueStruct {
    int process_num;
    Queue* process_queue[4];
} RQ;

void QueueInit(Queue *_q, int _time_quantum) {
    _q = (Queue*)malloc(sizeof(Queue));
    _q->front = NULL;
    _q->rear = NULL;
    _q->count = 0;
    _q->time_quantum = _time_quantum;
}

void ReadyQueueInit(RQ *_rq) {
    QueueInit(_rq->process_queue[0], 1);
    QueueInit(_rq->process_queue[1], 2);
    QueueInit(_rq->process_queue[2], 4);
    QueueInit(_rq->process_queue[3], 0);
}

void ReadyQueueFree(RQ *_rq) {
    for(int i=0; i<4; i++){
        free(_rq->process_queue[i]);
    }
}

void QueuePop(RQ *_rq, int _qnum) {
    Queue *q = _rq->process_queue[_qnum];
    if(q->count == 0) {
        return;
    }
    Queue *tmp = q->front;
    q->front = q->front->next;
    if(q->count == 1){
        q->rear = NULL;
    }
    free(tmp);
    --(q->count);
}

void QueuePush(RQ *_rq, int _qnum, Process *_p) {
    QNode *new_node = (QNode*)malloc(sizeof(Queue));
    new_node->next = NULL;
    new_node->p = _p;

    Queue *q = _rq->process_queue[_qnum];

    if(q->count == 0) { // empty queue;
        q->front = new_node;
        q->rear = new_node;
    }
    else{
        q->rear->next = new_node;
        q->rear = new_node;
    }
    ++(q->count);
}

Process* QueueFront(RQ *_rq, int _qnum){
    if(_rq->process_queue[_qnum]->count == 0){
        return NULL;
    }
    return _rq->process_queue[_qnum]->front->p;
}

int main(void) {
    int pNum;
    RQ *ready_queue = (RQ*)malloc(sizeof(RQ));
    ReadyQueueInit(ready_queue);
    
    scanf("%d", &pNum);
    getchar();
    Process *pid = (Process*)malloc(sizeof(Process) * pNum);
    for(int i=0; i<pNum; i++){
        scanf("%d %d %d %d ", &pid[i].pid, &pid[i].arrival_time, &pid[i].queue_pos, &pid[i].cycles);
        pid[i].cpu_burst_arr = (int*)malloc(sizeof(int) * pid[i].cycles);
        pid[i].io_burst_arr = (int*)malloc(sizeof(int) * pid[i].cycles);
        for(int j=0; j<pid[i].cycles * 2 - 1; j++){
            int burst;
            scanf("%d", &burst);
            getchar();
            if(j % 2 == 0){
                pid[i].cpu_burst_arr[j/2] = burst;
            }
            else{
                pid[i].io_burst_arr[j/2] = burst;
            }
        }
    }



    free(ready_queue);
}