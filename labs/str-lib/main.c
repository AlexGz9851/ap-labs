#include <stdio.h>
#include "strlib.h"

int main(int argc, char** argv){
    printf("Initial Lenght\t: %d\n",mystrlen(argv[1]));
    char * added = mystradd(argv[1], argv[2]);
    printf("New String\t: %s\n", added);
    printf("Substring was found\t: %s\n",mystrfind(added, argv[3])?"Yes":"No");
    return 0;
}
