#include <stdio.h>
#include "logger.h"

int main(){

    infof("INFO Message %d\n", 1);
    warnf("WARN Message %d\n", 2);
    errorf("ERROR Message %d\n", 2);
    panicf("PANIC AT THE DISCO\n");

    return 0;
}
