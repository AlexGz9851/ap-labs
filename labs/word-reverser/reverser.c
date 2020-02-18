#include <stdio.h>
#include <stdlib.h>


int reverse(int c){
    //printf("c%d\n",c);
    if(c==10) return 0; //NL
    if(c==-1) return 1;//EOF
    int x = getchar();
    int answer = reverse(x);
    printf("%c\0",(char)c);//here happens
    //fflush(stdin);
    return answer;
}

int main(){
    // Place your magic here
    printf("Inversor de palabras:\n");
    int first;
    int exitCode;
    while  ((first = getchar() ) != -1 ){
        exitCode = reverse(first);
        printf(" \n");
        if(exitCode) break;
    }
    return 0;
}