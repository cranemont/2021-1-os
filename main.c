#include<stdio.h>
#include<stdlib.h>

typedef struct ProcessStruct {
    int pid;
    int arrival_time;
    int queue_pos;
    int cycles;
    int *cpu_burst_arr;
    int *io_burst_arr;
    int process_start_time;
    int process_end_time;
} Process;

int processing_pid;
int* asleep_pid;
int timer;
int processing_time;
Process* pid;
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
RQ *rq;

void QueueInit(Queue *_q, int _time_quantum) {
    _q = (Queue*)malloc(sizeof(Queue));
    _q->front = NULL;
    _q->rear = NULL;
    _q->count = 0;
    _q->time_quantum = _time_quantum;
}

void ReadyQueueInit() {
    QueueInit(rq->process_queue[0], 1);
    QueueInit(rq->process_queue[1], 2);
    QueueInit(rq->process_queue[2], 4);
    QueueInit(rq->process_queue[3], 0);
}

void ReadyQueueFree() {
    for(int i=0; i<4; i++){
        free(rq->process_queue[i]);
    }
}

void QueuePop(int _qnum) {
    Queue *q = rq->process_queue[_qnum];
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

void RunProcess(Process *_p) {

}

void QueuePush(int _qnum, Process *_p) {
    QNode *new_node = (QNode*)malloc(sizeof(Queue));
    new_node->next = NULL;
    new_node->p = _p;

    Queue *q = rq->process_queue[_qnum];

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

Process* QueueFront(int _qnum){
    if(rq->process_queue[_qnum]->count == 0){
        return NULL;
    }
    return rq->process_queue[_qnum]->front->p;
}

void TimeTicker(int _pNum){
    for(int i=0; i<_pNum; i++){
        if(pid[i].arrival_time == timer){
            QueuePush(pid[i].queue_pos , &pid[i]);
        }
    }

    if(rq->process_queue[pid[processing_pid].queue_pos]->time_quantum == processing_time){

    }



    //처리하고나면 0
    for(int i=0; i<_pNum; i++){
        if(asleep_pid[i] != -1){
            int pid_n = asleep_pid[i];
            int cur_cycle = pid[pid_n].cycles;
            if(pid[pid_n].io_burst_arr[cur_cycle] == 0){
                if(cur_cycle == 0){
                    //done process;
                    continue;
                }
                else{
                    //interrupt;
                    if(pid[pid_n].queue_pos != 0){
                        pid[pid_n].queue_pos -= 1;
                    }
                    QueuePush(pid[pid_n].queue_pos, &pid[pid_n]);
                    pid[pid_n].cycles -= 1;
                }
            }
            
            pid[pid_n].io_burst_arr[cur_cycle] -= 1;
        }
    }
    processing_time++;
    timer++;
}

int main(void) {
    int pNum;
    rq = (RQ*)malloc(sizeof(RQ));
    ReadyQueueInit(rq);
    timer = 0;
    processing_time = 0;

    scanf("%d", &pNum);
    getchar();
    pid = (Process*)malloc(sizeof(Process) * pNum);
    asleep_pid = (int*)malloc(sizeof(int) * pNum);
    for(int i=0; i<pNum; i++){
        asleep_pid[i] = -1;
    }

    for(int i=0; i<pNum; i++){
        int _cycle;
        scanf("%d %d %d %d ", &pid[i].pid, &pid[i].arrival_time, &pid[i].queue_pos, &_cycle);
        pid[i].cycles = _cycle;
        pid[i].cpu_burst_arr = (int*)malloc(sizeof(int) * _cycle);
        pid[i].io_burst_arr = (int*)malloc(sizeof(int) * _cycle);
        for(int j=0; j<_cycle * 2 - 1; j++){
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
        pid[i].io_burst_arr[_cycle - 1] = 0;

        if(rq->process_queue[pid[i].queue_pos]->count == 0){
            QueuePush(pid[i].queue_pos, &pid[i]);
        }
        else{
            Queue *tmp = rq->process_queue[pid[i].queue_pos]->front;
        }
        pid[i].process_start_time = -1;
    }

    free(rq);
}