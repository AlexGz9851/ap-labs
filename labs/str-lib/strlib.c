#include <stdio.h>
#include <stdlib.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int mystrlen(char *str){
    int count=0;
    while(str[count]!='\0') count++; 
    return count;
}

int mystrfind(char *origin, char *substr){
    int found = 1;
    int lenOr = mystrlen(origin);
    int lenS = mystrlen(substr);
    if(lenOr < lenS) return 0;
    for(int i=0; i<lenOr+1-lenS;i++){
        found = 1;
        for(int j=0;j<lenS;j++){
            if(substr[j]!=origin[j+i]){
                found = 0;
                break;
            }
        }
        if(found)return 1;
    }
    return 0;
}

char *mystradd(char *origin, char *addition){
    int lenOrigin, lenAddition;
    lenAddition = mystrlen(addition);
    lenOrigin = mystrlen(origin);
    char * result = (char *) malloc((lenOrigin+lenAddition+1)*sizeof(char *));
    int i=0;
    while(i<lenOrigin){
        result[i] = origin[i];
        i++;
    }
    while(i-lenOrigin<lenAddition){
        result[i] = addition[i-lenOrigin];
        i++;
    }
    result[lenAddition+lenOrigin] = '\0';
    return result;
}

int main(int argc, char** argv){
    printf("len %d\n",mystrlen(mystradd(argv[1], argv[2])));
    printf("%s\n",mystradd(argv[1], argv[2]));
    printf("%s\n", mystrfind(argv[1], argv[2])?"Yes":"No");
}
