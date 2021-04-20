#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct _process{
    int *reqestedResource;
    int *allocatedResource;
    bool available;
} processStruct;

int* resourceUnit;
int resourceTypes;
int processNum;

processStruct* pid;

int findReductivePid(){

    for(int i=0; i<processNum; i++){
        int flag = 0;
        
        if(!pid[i].available){
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
    pid[_pid].available = false;
    for(int i=0; i<resourceTypes; i++){
        resourceUnit[i] += pid[_pid].allocatedResource[i];
    }
}

int graphReduction(){
    // printf("ResourceUnits: ");
    // for(int i=0; i<resourceTypes; i++){
    //     printf("%d\t",resourceUnit[i]);
    // }
    // printf("\n");
    int reductivdPid = findReductivePid();
    if(reductivdPid == -1){ 
        return 0;
    }
    freeProcess(reductivdPid);
    return 1;
}

int main(void){

    FILE* input = fopen("input.txt", "r");

    if((EOF == fscanf(input, "%d %d ", &processNum, &resourceTypes))){
        printf("INVALID INPUT");
        return 0;
    }

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
        pid[i].available = true;
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

    while(graphReduction());
    
    int flag = 0;
    for(int i=0; i<processNum; i++){
        if(pid[i].available == true){
            flag = 1;
            break;
        }
    }

    if(flag == 0){
        printf("Not in a deadlock state\n");
    }
    else{
        printf("Deadlocked process list\n");
        for(int i=0; i<processNum; i++){
            if(pid[i].available == true){
                printf("P%d ", i);
            }
        }
        printf("\n");
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
    for(int i=0; i<processNum; i++){
        free(pid[i].allocatedResource);
        free(pid[i].reqestedResource);
    }
    free(pid);
    free(resourceUnit);
    return 0;
}