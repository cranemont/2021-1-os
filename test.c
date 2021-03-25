#include<stdio.h>
int main(void){
    char a = (char)1;
    if(a == 1){
        printf("%d %c\n",a,a);
    }
    switch (a)
    {
    case 1:
        printf(">>>");
        break;
    
    default:
        break;
    }
    return 0;
}