#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    if(argc==2){
        int f = atoi(argv[1]);
        printf("Farenheit: %3i    Celsius: %3.1f\n",f, (5.0/9.0)*(f-32.0));
        return 0;
    }else{
        for(int i=atoi(argv[1]);i<atoi(argv[2]);i+=atoi(argv[3])){
            printf("Farenheit: %3i    Celsius: %3.1f\n",i, (5.0/9.0)*(i-32.0));
        }
    }
}