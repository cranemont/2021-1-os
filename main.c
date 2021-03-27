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
    int burst_acc;
    int process_end_time;
    int state; // -1: before arrival / 0: in queue / 1: CPU burst / 2: I/O burst / 3: process done
} Process;

typedef struct QNodeStruct {
    int q_pid;
    struct QNodeStruct *next;
} QNode;

typedef struct QueueStruct {
    QNode *front;
    QNode *rear;
    int count;
    int time_quantum; // for RR
} Queue;

Queue* ready_queue[4];
int processing_pid;
int* asleep_pid;
int timer;
int p_num;
int running;
int flag_zero_io_time;
Process* pid;

Queue* QueueInit(int _time_quantum) {
    Queue* _q = (Queue*)malloc(sizeof(Queue));
    _q->front = NULL;
    _q->rear = NULL;
    _q->count = 0;
    _q->time_quantum = _time_quantum;
    return _q;
}

void ReadyQueueInit() {
    ready_queue[0] = QueueInit(1);
    ready_queue[1] = QueueInit(2);
    ready_queue[2] = QueueInit(4);
    ready_queue[3] = QueueInit(-1);
}

void ReadyQueueFree() {
    for(int i=0; i<4; i++){
        free(ready_queue[i]);
    }
}

void QueuePop(int _qnum) {
    Queue *q = ready_queue[_qnum];
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

void QueuePush(int _qnum, int pid_num) {
    QNode *new_node = (QNode*)malloc(sizeof(QNode));
    new_node->next = NULL;
    new_node->q_pid = pid_num;
    Queue *q = ready_queue[_qnum];
    
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
    if(ready_queue[_qnum]->count == 0){
        return -1;
    }
    return ready_queue[_qnum]->front->q_pid;
}

int CPUCalc(){
    int queue_pos = pid[processing_pid].queue_pos;

    //현재 processing중인거 확인 후 처리. 근데 FCFS Q3는???
    if(ready_queue[queue_pos]->time_quantum == pid[processing_pid].runtime){
        pid[processing_pid].runtime = 0;
        if(pid[processing_pid].cpu_burst_arr[pid[processing_pid].cur_cycle] != 0){ // 타임퀀텀이 그냥 끝남(해당 큐의)(cpu time이 남았는데)
            pid[processing_pid].queue_pos += 1;
            QueuePush(pid[processing_pid].queue_pos, processing_pid);
            pid[processing_pid].state = 0;
        }
        else{ // CPU타임이 끝나서 Asleep 상태로. //io가 0이면 asleep 보낼 필요 없다.
            if(pid[processing_pid].io_burst_arr[pid[processing_pid].cur_cycle] == 0){
                flag_zero_io_time = 1;
                //pid[processing_pid].queue_pos += 1;//time quantum 다 써도 not preempted.
            }
            asleep_pid[processing_pid] = processing_pid;
            pid[processing_pid].state = 2;
        }
        processing_pid = -1;
        return -1;
    }
    else if(pid[processing_pid].cpu_burst_arr[pid[processing_pid].cur_cycle] == 0){ // 현재 큐의 타임퀀텀이 남았는데 cpu burst가 끝나서 I/O asleep 상태 돌입. Q3 포함.
        if(pid[processing_pid].io_burst_arr[pid[processing_pid].cur_cycle] == 0){ // 그냥 cpu타임이 처음부터 0이었을때도 여기로옴
            flag_zero_io_time = 1;
        }
        pid[processing_pid].runtime = 0;
        asleep_pid[processing_pid] = processing_pid;
        pid[processing_pid].state = 2;
        processing_pid = -1;
        return -1;
    }
    else{ //there is a running process
        return 1;
    }
}

void IOCalc(){
    for(int i=0; i<p_num; i++){
        if(asleep_pid[i] != -1){
            int pid_asleep = asleep_pid[i];
            int cycle = pid[pid_asleep].cur_cycle;
            if(pid[pid_asleep].io_burst_arr[cycle] == 0){ // I/O done
                asleep_pid[i] = -1;

                if(cycle == pid[pid_asleep].cycles - 1){ // Process done
                    pid[pid_asleep].state = 3;
                    pid[pid_asleep].process_end_time = timer;
                    continue;
                }
                else{ // interrupt
                    if(pid[pid_asleep].queue_pos != 0 && pid[pid_asleep].queue_pos != 3 && flag_zero_io_time == 0){
                        pid[pid_asleep].queue_pos -= 1;
                    }
                    flag_zero_io_time = 0;
                    // 만약 다음 cpu사이클이 0이면 바로 io처리로 다시와야한다.?
                    // 큐에서 대기할 이유가 있을까? 대기하는걸로 가정.
                    QueuePush(pid[pid_asleep].queue_pos, pid_asleep);
                    pid[pid_asleep].state = 0;
                    pid[pid_asleep].cur_cycle += 1;
                }
            }
        }
    }
}

int NewProcess(){
    int flag = 0; // 그대로 0이면 추가프로세스 없음
    for(int i=0; i<4; i++){
        if(ready_queue[i]->count != 0) {
            processing_pid = QueueFront(i);//ready_queue[i]->front->q_pid; // QueueFront(i);
            pid[processing_pid].state = 1;
            QueuePop(i);
            flag = 1;
            break;
        }
    }
    if(flag == 0){
        return -1;
    }
    return 1;
}

int TimeTicker(FILE *fp){
    
    for(int i=0; i<p_num; i++){
        if(pid[i].arrival_time == timer){
            QueuePush(pid[i].queue_pos , i);
            pid[i].state = 0;
        }
    }

    IOCalc(); 

ZeroCPUTime:
    if(running != -1){ // if there is a running process
        running = CPUCalc(); // io가 0인건 다음 iocalc가 처리
    }
    IOCalc();
    if(running == -1){
        running = NewProcess();
        if(running == 1 && pid[processing_pid].cpu_burst_arr[pid[processing_pid].cur_cycle] == 0){
             // 새로받은 cpu타임이 0인 특수한상황.
            goto ZeroCPUTime;
        }
    }

    if(running == 1){ // running이 애초부터 1이면 newprocess 안하고 기존 실행하던거. newprocess면 어차피 제일우선순위높은놈 선택됨
        
        pid[processing_pid].runtime++;
        pid[processing_pid].cpu_burst_arr[pid[processing_pid].cur_cycle] -= 1;  
    }
    
    for(int i=0; i<p_num; i++){
        if(asleep_pid[i] != -1){
            pid[asleep_pid[i]].io_burst_arr[pid[asleep_pid[i]].cur_cycle] -= 1;
        }
    }

    timer++;
    for(int i=0; i<p_num; i++){
        switch (pid[i].state)
        {
        case 0:
            fprintf(fp, "Q%d\t", pid[i].queue_pos);
            break;
        case 1:
            fprintf(fp, "|%d|\t", pid[i].queue_pos);
            break;
        case 2:
            fprintf(fp, ":::\t");
            break;
        default: // -1 or 3
            fprintf(fp, "\t");
            break;
        }
    }
    fprintf(fp, "\n");
    for(int i=0; i<p_num; i++){
        fprintf(fp, "--------");
    }
    fprintf(fp, " %d\n", timer);

    int done = 1;
    for(int i=0; i<p_num; i++){
        if(pid[i].state != 3){
            done = 0;
            break;
        }
    }
    if(done == 1){
        return 0;
    }
    
    return 1;
}

int main(int argc, char *argv[]) {
    FILE* input = fopen(argv[1], "r");
    ReadyQueueInit();
    timer = 0;
    processing_pid = -1;
    running = -1;
    fscanf(input, "%d", &p_num);
    fgetc(input);
    pid = (Process*)malloc(sizeof(Process) * p_num);
    asleep_pid = (int*)malloc(sizeof(int) * p_num);
    for(int i=0; i<p_num; i++){
        asleep_pid[i] = -1;
    }
    
    flag_zero_io_time = 0;
    for(int i=0; i<p_num; i++){
        int _cycle;
        int at;
        fscanf(input, "%d %d %d %d ", &pid[i].pid, &at, &pid[i].queue_pos, &_cycle);
        pid[i].arrival_time = at;
        pid[i].pid -= 1;
        pid[i].state = -1;
        pid[i].cycles = _cycle;
        pid[i].cur_cycle = 0;
        pid[i].runtime = 0;
        pid[i].cpu_burst_arr = (int*)malloc(sizeof(int) * _cycle);
        pid[i].io_burst_arr = (int*)malloc(sizeof(int) * _cycle);
        pid[i].burst_acc = 0;
        for(int j=0; j<_cycle * 2 - 1; j++){
            int burst;
            fscanf(input, "%d", &burst);
            fgetc(input);
            if(j % 2 == 0){
                pid[i].cpu_burst_arr[j/2] = burst;
                pid[i].burst_acc += burst;
            }
            else{
                pid[i].io_burst_arr[j/2] = burst;
            }
        }
        pid[i].io_burst_arr[_cycle - 1] = 0;
    }
    
    fclose(input);
    FILE *output = fopen("out.txt", "w");
    for(int i=0; i<p_num; i++){
        fprintf(output, "P%d\t", i+1);
    }
    fprintf(output, "\n");
    for(int i=0; i<p_num; i++){
        fprintf(output, "--------");
    }
    fprintf(output, " 0\n");
    while(TimeTicker(output));
    fprintf(output, "\n");
    int avg_wt = 0;
    int avg_tt = 0;
    fprintf(output,"+-------+-------+-------+\n");
    fprintf(output,"|P#\t|WT\t|TT\t|\n");
    fprintf(output,"+-------+-------+-------+\n");
    for(int i=0; i<p_num; i++){
        int tt = pid[i].process_end_time - pid[i].arrival_time;
        int wt = tt - pid[i].burst_acc;
        fprintf(output,"|P%d\t", i+1);
        fprintf(output,"|%d\t|%d\t|\n", wt, tt);
        avg_wt += wt;
        avg_tt += tt;
    }
    fprintf(output, "+-------+-------+-------+\n\n");
    fprintf(output, "Average Wait Time: %.4f\nAverage Turnaround Time: %.4f\n", avg_wt/(float)p_num, avg_tt/(float)p_num);
    fclose(output);
    ReadyQueueFree();

}