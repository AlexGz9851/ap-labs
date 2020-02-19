#include <stdio.h>
#include <stdlib.h>


int reverse(int c){
    if(c=='\n'|| c==EOF) return c==EOF;
    int answer = reverse(getchar());
    printf("%c",(char)c);
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