#include<stdio.h>
#include<stdlib.h>

typedef struct ProcessStruct {
    int pid;
    int runtime;
    int arrival_time;
    int queue_pos;
    int cycles;
    int cur_cycle;
    int *cpu_burst_arr;
    int *io_burst_arr;
    int process_start_time;
    int process_end_time;
} Process;

int processing_pid;
int* asleep_pid;
int timer;
int processing_time;
int p_num;
int running;
Process* pid;

typedef struct QNodeStruct {
    int q_pid;//Process *p; // int pid?
    struct QNodeStruct *next;
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

Queue* QueueInit(int _time_quantum) {
    Queue* _q = (Queue*)malloc(sizeof(Queue));
    _q->front = NULL;
    _q->rear = NULL;
    _q->count = 0;
    _q->time_quantum = _time_quantum;
    return _q;
}

void ReadyQueueInit() {
    rq->process_queue[0] = QueueInit(1);
    rq->process_queue[1] = QueueInit(2);
    rq->process_queue[2] = QueueInit(4);
    rq->process_queue[3] = QueueInit(-1);
    // QueueInit(rq->process_queue[1], 2);
    // QueueInit(rq->process_queue[2], 4);
    // QueueInit(rq->process_queue[3], -1);
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
    QNode *tmp = q->front;
    q->front = q->front->next;
    if(q->count == 1){
        q->rear = NULL;
    }
    free(tmp);
    --(q->count);
}

void RunProcess(Process *_p) {

}

void QueuePush(int _qnum, int pid_num) {
    QNode *new_node = (QNode*)malloc(sizeof(QNode));
    new_node->next = NULL;
    new_node->q_pid = pid_num;
    // printf("here push\n");
    Queue *q = rq->process_queue[_qnum];
    // printf("22\n");
    
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

int QueueFront(int _qnum){
    if(rq->process_queue[_qnum]->count == 0){
        return -1;
    }
    return rq->process_queue[_qnum]->front->q_pid;
}

int CPUCalc(){
    int queue_pos = pid[processing_pid].queue_pos;

    // if(queue_pos == 3){
    //     // cpu burst 끝나서 I/O burst로
    // }

    //현재 processing중인거 확인 후 처리. 근데 FCFS Q3는???
    if(rq->process_queue[queue_pos]->time_quantum == pid[processing_pid].runtime){
        pid[processing_pid].runtime = 0;
        if(pid[processing_pid].cpu_burst_arr[pid[processing_pid].cur_cycle] != 0){ // 타임퀀텀이 그냥 끝남(해당 큐의)(cpu time이 남았는데)
            // if(pid[processing_pid].queue_pos != 3){ 어차피 여기는 Q3 안들어옴
            //     pid[processing_pid].queue_pos += 1;
            // }
            pid[processing_pid].queue_pos += 1;
            if(pid[processing_pid].queue_pos == 3){
                printf("3333\n");
            }
            QueuePush(pid[processing_pid].queue_pos, processing_pid);
        }
        else{ // CPU타임이 끝나서 Asleep 상태로. //io가 0이면 asleep 보낼 필요 없다.
            for(int i=0; i<p_num; i++){
                if(asleep_pid[i] == -1){
                    asleep_pid[i] = processing_pid;
                    if(pid[processing_pid].io_burst_arr[pid[processing_pid].cur_cycle] != 0){
                        pid[processing_pid].io_burst_arr[pid[processing_pid].cur_cycle] -= 1;
                    }
                    break;
                }
            }
        }
        processing_pid = -1;
    }
    else if(pid[processing_pid].cpu_burst_arr[pid[processing_pid].cur_cycle] == 0){ // 현재 큐의 타임퀀텀이 남았는데 cpu burst가 끝나서 I/O asleep 상태 돌입. Q3 포함.
        pid[processing_pid].runtime = 0;
        for(int i=0; i<p_num; i++){
            if(asleep_pid[i] == -1){
                asleep_pid[i] = processing_pid;
                if(pid[processing_pid].io_burst_arr[pid[processing_pid].cur_cycle] != 0){
                    pid[processing_pid].io_burst_arr[pid[processing_pid].cur_cycle] -= 1;
                }
                
                processing_pid = -1;
                break;
            }
        }
    }
    else{ //there is a running process
        return 1;
    }
    return -1;
}

void IOCalc(){
    for(int i=0; i<p_num; i++){
        if(asleep_pid[i] != -1){
            int pid_asleep = asleep_pid[i];
            int cycle = pid[pid_asleep].cur_cycle;
            if(pid[pid_asleep].io_burst_arr[cycle] == 0){ // I/O done
                asleep_pid[i] = -1;

                if(cycle == pid[pid_asleep].cycles - 1){ // Process done
                    pid[pid_asleep].process_end_time = timer;
                    continue;
                }
                else{ // interrupt
                    if(pid[pid_asleep].queue_pos != 0){
                        pid[pid_asleep].queue_pos -= 1;
                    }
                    QueuePush(pid[pid_asleep].queue_pos, pid_asleep);
                    pid[pid_asleep].cur_cycle += 1;
                }
            }
            
            pid[pid_asleep].io_burst_arr[cycle] -= 1;
        }
    }
}

int NewProcess(){
    int flag = 0; // 그대로 0이면 추가프로세스 없음
    for(int i=0; i<4; i++){
        if(rq->process_queue[i]->count != 0) {
            // printf("here 1\n");
            processing_pid = QueueFront(i);//rq->process_queue[i]->front->q_pid; // QueueFront(i);
            // processing_pid = link->pid;
            // printf("here 2\n");
            QueuePop(i);
            // printf("here 3\n");
            if(pid[processing_pid].process_start_time == -1){
                pid[processing_pid].process_start_time = timer;
            }
            flag = 1;
            break;
        }
    }
    if(flag == 0){
        return -1;
    }
    return 1;
}
void InAsleep(){
    for(int i=0; i<p_num; i++){
        printf("%d\t", asleep_pid[i]);
    }
    printf("\n");
}
void RQPrint(){
    printf("TIME: %d\t", timer);
    printf("RUNNING: %d\n", processing_pid);
    for(int i=0; i<4; i++){
        printf("QUEUE %d: ", i);
        for(int j=0; j<rq->process_queue[i]->count; j++){
            QNode *prn = rq->process_queue[i]->front;
            printf("%d\t", prn->q_pid);
            prn = prn->next;
        }
        printf("\n");
    }
    InAsleep();
}

int IOCheck(){
    for(int i=0; i<p_num; i++){
        if(asleep_pid[i] != -1){
            return 1;
        }
    }
    return -1;
}

int TimeTicker(){
    //AT 처리
    RQPrint();
    for(int i=0; i<p_num; i++){
        if(pid[i].arrival_time == timer){
           // printf("here\n");
            QueuePush(pid[i].queue_pos , i);
        }
    } //printf("1\n");
   
    //현재 processing중인거 확인 후 처리. 근데 FCFS Q3는???
    IOCalc();  //printf("2\n"); 
    if(processing_pid != -1){ // for init.
        running = CPUCalc(); 
    }
    
    // printf("3\n");
     //처리하고나면 asleep에서 제외
    
    
    
    //RQ에서 processing할거 찾아서 넣기 (큐에 넣을거 다 넣은 이후) processing pid 교체.
    // 처음 프로세스 들어갈때 start time 없으면(-1) 기록하기
    if(running == -1){
        running = NewProcess();
    }

    if(running == 1){
        pid[processing_pid].runtime++;
        // printf("%d\n", processing_pid);
        // printf("%d\n", pid[processing_pid].cur_cycle);
        pid[processing_pid].cpu_burst_arr[pid[processing_pid].cur_cycle] -= 1;
    }
    

    
    // processing_time++;
    timer++;
    
    if(running == -1 && IOCheck() == -1){
        return 0;
    }
    // printf("4\n");
    return 1;
}

int main(int argc, char *argv[]) {
    FILE* input = fopen(argv[1], "r");
    rq = (RQ*)malloc(sizeof(RQ));
    ReadyQueueInit(rq);
    timer = 0;
    processing_time = 0;
    processing_pid = -1;
    running = -1;
    fscanf(input, "%d", &p_num);
    fgetc(input);
    pid = (Process*)malloc(sizeof(Process) * p_num);
    asleep_pid = (int*)malloc(sizeof(int) * p_num);
    for(int i=0; i<p_num; i++){
        asleep_pid[i] = -1;
    }
 
    for(int i=0; i<p_num; i++){
        int _cycle;
        fscanf(input, "%d %d %d %d ", &pid[i].pid, &pid[i].arrival_time, &pid[i].queue_pos, &_cycle);
        pid[i].pid -= 1;
        pid[i].cycles = _cycle;
        pid[i].cur_cycle = 0;
        pid[i].runtime = 0;
        pid[i].cpu_burst_arr = (int*)malloc(sizeof(int) * _cycle);
        pid[i].io_burst_arr = (int*)malloc(sizeof(int) * _cycle);
        for(int j=0; j<_cycle * 2 - 1; j++){
            int burst;
            fscanf(input, "%d", &burst);
            fgetc(input);
            if(j % 2 == 0){
                pid[i].cpu_burst_arr[j/2] = burst;
            }
            else{
                pid[i].io_burst_arr[j/2] = burst;
            }
        }
        pid[i].io_burst_arr[_cycle - 1] = 0;

        // if(rq->process_queue[pid[i].queue_pos]->count == 0){
        //     QueuePush(pid[i].queue_pos, &pid[i]);
        // }
        // else{
        //     Queue *tmp = rq->process_queue[pid[i].queue_pos]->front;
        // }
        pid[i].process_start_time = -1; // 시작 시간 기록용
    }
    
    fclose(input);
    while(TimeTicker()) {printf("\n");}

    ReadyQueueFree();
    free(rq);
}