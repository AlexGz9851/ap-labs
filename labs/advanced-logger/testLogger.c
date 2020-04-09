#include <stdio.h>
#include "logger.h"

int main(){

    // default logging
    infof("INFO Message %d\n", 1);
    warnf("WARN Message %d\n", 2);
    errorf("ERROR Message %d\n", 2);

    // stdout logging
    initLogger("stdout");
    infof("INFO Message %d\n", 1);
    warnf("WARN Message %d\n", 2);
    errorf("ERROR Message %d\n", 2);
    panicf("ERROR Message %d\n", 32);

    // syslog logging
    initLogger("syslog");
    infof("INFO Message %d\n", 11);
    warnf("WARN Message %d\n", 22);
    errorf("ERROR Message %d\n", 32);
    panicf("Panic at the syslog %d\n", 32);

    return 0;
}
