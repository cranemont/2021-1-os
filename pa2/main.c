#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct processStruct{
    int *reqestedResource;
    int *allocatedResource;
    bool available;
} processStruct;

int* resourceUnit;
int* availableResource;
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
            if(pid[i].reqestedResource[j] > availableResource[j]){
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
        availableResource[i] += pid[_pid].allocatedResource[i];
    }
}

int graphReduction(){
    int reductivePid = findReductivePid();
    if(reductivePid == -1){ 
        return 0;
    }
    printf("\n");
    printf("Reducted process: %d\n", reductivePid+1);
    printf("P%d request resources: ", reductivePid+1);
    for(int i=0; i<resourceTypes; i++){
        printf("\t%d", pid[reductivePid].reqestedResource[i]);
    }
    printf("\n");
    printf("P%d allocate resources: ", reductivePid+1);
    for(int i=0; i<resourceTypes; i++){
        printf("\t%d", pid[reductivePid].allocatedResource[i]);
    }
    printf("\n\n");
    freeProcess(reductivePid);
    printf("Available resources: ");
    for(int i=0; i<resourceTypes; i++){
        printf("\t%d", availableResource[i]);
    }
    return 1;
}

int main(void){

    FILE* input = fopen("input.txt", "r");

    if((EOF == fscanf(input, "%d %d ", &processNum, &resourceTypes))){
        printf("INVALID INPUT");
        return 0;
    }
    if(processNum < 0 || resourceTypes < 0){
        printf("INVALID INPUT");
        return 0;
    }

    pid = (processStruct*)malloc(sizeof(processStruct)*processNum);
    resourceUnit = (int*)malloc(sizeof(int)*resourceTypes);
    availableResource = (int*)malloc(sizeof(int)*resourceTypes);
    for(int i=0; i<resourceTypes; i++){
        if((EOF == fscanf(input, "%d ", &resourceUnit[i]))){
            printf("INVALID INPUT");
            return 0;
        }
        availableResource[i] = resourceUnit[i];
    }

    for(int i=0; i<processNum; i++){
        pid[i].allocatedResource = (int*)malloc(sizeof(int)*resourceTypes);
        pid[i].available = true;
        for(int j=0; j<resourceTypes; j++){
            if((EOF == fscanf(input, "%d ", &pid[i].allocatedResource[j]))){
                printf("INVALID INPUT");
                return 0;
            }

            if((availableResource[j] -= pid[i].allocatedResource[j]) < 0 || pid[i].allocatedResource[j] < 0){
                printf("INVALID INPUT alloc");    
                return 0;
            };
        }
    }

    for(int i=0; i<processNum; i++){
        pid[i].reqestedResource = (int*)malloc(sizeof(int)*resourceTypes);

        for(int j=0; j<resourceTypes; j++){
            if((EOF == fscanf(input, "%d ", &pid[i].reqestedResource[j]))){
                printf("INVALID INPUT");
                return 0;
            }

            if(resourceUnit[j] < pid[i].reqestedResource[j] + pid[i].allocatedResource[j] || pid[i].reqestedResource[j] < 0){
                printf("INVALID INPUT request");
                return 0;
            }
        }
    }
    if(fgetc(input) != EOF){
        printf("INVALID INPUT");
        return 0;
    }
    fclose(input);
    printf("Available resources: ");
    for(int i=0; i<resourceTypes; i++){
        printf("\t%d", availableResource[i]);
    }
    while(graphReduction());
    printf("\n");
    int flag = 0;
    for(int i=0; i<processNum; i++){
        if(pid[i].available == true){
            flag = 1;
            break;
        }
    }

    if(flag == 0){
        printf("\n*****Non-deadlock State*****\n");
    }
    else{
        printf("\n*****Deadlocked process list*****\n");
        for(int i=0; i<processNum; i++){
            if(pid[i].available == true){
                printf("P%d\n", i+1);
                printf("Request resources: ");
                for(int j=0; j<resourceTypes; j++){
                    printf("\t%d", pid[i].reqestedResource[j]);
                }
                printf("\n");
            }
        }
        printf("\n");
    }
    // printf("#Ps: %d\t#R.types: %d\n", processNum, resourceTypes);
    // printf("ResourceUnits: ");
    // for(int i=0; i<resourceTypes; i++){
    //     printf("%d\t",resourceUnit[i]);
    // }
    // printf("\n");

    // for(int i=0; i<processNum; i++){
    //     printf("Process#%d alloc: ",i);
    //     for(int j=0; j<resourceTypes; j++){
    //         printf("%d\t",pid[i].allocatedResource[j]);
    //     }
    //     printf("\n");
    // }

    // printf("\n");
    // for(int i=0; i<processNum; i++){
    //     printf("Process#%d req: ",i);
    //     for(int j=0; j<resourceTypes; j++){
    //         printf("%d\t",pid[i].reqestedResource[j]);
    //     }
    //     printf("\n");
    // }

    for(int i=0; i<processNum; i++){
        free(pid[i].allocatedResource);
        free(pid[i].reqestedResource);
    }
    free(pid);
    free(resourceUnit);
    free(availableResource);
    return 0;
}