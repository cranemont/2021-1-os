// 스택 이용

// 프로세스 리스트 돌면서 <제거 가능한거> 스택에 쌓고
// <리소스 계산>
// 만약 제거 가능한 프로세스가 없으면
//      스택이 empty면 전부 blocked. 종료

//      empty가 아니면
//      스택의 top 돌려놓고 해당 프로세스 제외하고 <제거 가능한거> 확인
//      있으면
//          스택에서 pop하고 제거 가능한거 push, <리소스 계산>, continue
//      없으면
//          blocked. 스택에 없는것들은 전부 blocked state
//반복
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct _stackNode{
    int pid;
    struct _stackNode* prev;
} stackNode;

typedef struct _stack{
    int nodeNum;
    stackNode* top;
} stack;

void stackInit(stack* st){
    st->nodeNum = 0;
    st->top = NULL;
}

void stackPush(stack* st, int _pid){
    stackNode* newNode = (stackNode*)malloc(sizeof(stackNode));
    newNode->pid = _pid;
    newNode->prev = NULL;

    if(st->nodeNum == 0){
        st->top = newNode;
    }
    else{
        newNode->prev = st->top;
        st->top->prev = NULL;
        st->top = newNode;
    }
    st->nodeNum++;
}

void stackPop(stack* st){
    if(st->nodeNum == 0){
        return;
    }
    stackNode* tmp = st->top;
    st->top = st->top->prev;
    st->nodeNum--;
    free(tmp);
}
typedef struct _process{
    int *reqestedResource;
    int *allocatedResource;
    bool isAvailable;
} processStruct;

int* resourceUnit;
int resourceTypes;
int processNum;

processStruct* pid;
stack *st;

int findReductivePid(){

    for(int i=0; i<processNum; i++){
        int flag = 0;

        if(!pid[i].isAvailable){
            continue;
        }

        for(int j=0; j<resourceTypes; j++){
            if(pid[i].reqestedResource[j] > resourceUnit[j]){
                flag = 1;
                break;
            }
        }
        
        if(flag != 1){
            return i;
        }
    }

    return -1;
}

void freeProcess(int _pid){
    pid[_pid].isAvailable = false;
    for(int i=0; i<resourceTypes; i++){
        resourceUnit[i] += pid[_pid].allocatedResource[i];
    }
}

int graphReduction(){
    int reductivdPid = findReductivePid();
    if(reductivdPid == -1 && st->nodeNum == 0){
        return 0;
    }
}

int main(void){

    FILE* input = fopen("input.txt", "r");

    if((EOF == fscanf(input, "%d %d ", &processNum, &resourceTypes))){
        printf("INVALID INPUT");
        return 0;
    }

    st = (stack*)malloc(sizeof(stack));
    stackInit(st);
    pid = (processStruct*)malloc(sizeof(processStruct)*processNum);
    resourceUnit = (int*)malloc(sizeof(int)*resourceTypes);
    for(int i=0; i<resourceTypes; i++){
        if((EOF == fscanf(input, "%d", &resourceUnit[i]))){
            printf("INVALID INPUT");
            return 0;
        }

        if((fgetc(input) != '\n') && (i == resourceTypes-1)){
            printf("INVALID INPUT");    
            return 0;
        }
    }

    for(int i=0; i<processNum; i++){
        pid[i].allocatedResource = (int*)malloc(sizeof(int)*resourceTypes);
        pid[i].isAvailable = true;
        for(int j=0; j<resourceTypes; j++){
            if((EOF == fscanf(input, "%d", &pid[i].allocatedResource[j]))){
                printf("INVALID INPUT");
                return 0;
            }

            if((fgetc(input) != '\n') && (j == resourceTypes-1)){
                printf("INVALID INPUT alloc");    
                return 0;
            }

            if((resourceUnit[j] -= pid[i].allocatedResource[j]) < 0 || pid[i].allocatedResource[j] < 0){
                printf("INVALID INPUT alloc");    
                return 0;
            };
        }
    }

    for(int i=0; i<processNum; i++){
        pid[i].reqestedResource = (int*)malloc(sizeof(int)*resourceTypes);

        for(int j=0; j<resourceTypes; j++){
            if((EOF == fscanf(input, "%d", &pid[i].reqestedResource[j]))){
                printf("INVALID INPUT");
                return 0;
            }
            if((fgetc(input) != '\n') && (j == resourceTypes-1) && (i != processNum-1)){
                printf("INVALID INPUT req");    
                return 0;
            }
        }
    }

    printf("#Ps: %d\t#R.types: %d\n", processNum, resourceTypes);
    printf("ResourceUnits: ");
    for(int i=0; i<resourceTypes; i++){
        printf("%d\t",resourceUnit[i]);
    }
    printf("\n");

    for(int i=0; i<processNum; i++){
        printf("Process#%d alloc: ",i);
        for(int j=0; j<resourceTypes; j++){
            printf("%d\t",pid[i].allocatedResource[j]);
        }
        printf("\n");
    }

    printf("\n");
    for(int i=0; i<processNum; i++){
        printf("Process#%d req: ",i);
        for(int j=0; j<resourceTypes; j++){
            printf("%d\t",pid[i].reqestedResource[j]);
        }
        printf("\n");
    }

    
}