#include <stdio.h>
#include "logger.h"
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <signal.h>


#define RESET 0
#define BRIGHT 1
#define DIM 2
#define ITALICS 3 //These are the codes my virtual machine thrown.
#define UNDERLINE 4
#define BLINK 5
#define REVERSE 7
#define HIDDEN 8

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7
#define RESET_BG 8


void textcolor(int attr, int fg, int bg);
int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);
int initLogger(char *logType);

int isSyslog = 0;
int sysLogStatus=0;

int initLogger(char *logType) {
    if(strcmp(logType, "syslog") == 0){
        printf("Initializing Logger on: %s\n", logType);
        fflush(stdout);
        isSyslog = 1;
    }else if(   ( ! strcmp(logType, "stdout") )|| 
                ( ! strcmp(logType, "") )){
                printf("Initializing Logger on: %s\n", logType);
                fflush(stdout);
                isSyslog = 0;
    }else{
        printf("Invalid option. The possible options are: \"stdout\" and \"syslog\".\n");
        fflush(stdout);
        return -1;
    }
    return 0;
}

int printOn(const char *format, va_list args){
    if(isSyslog){
        openlog("myAdvancedLogger",LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(sysLogStatus,format,args);
        closelog();
    }else{ 
        vprintf(format, args); 
        fflush(stdout);
        }
    return 0;
}


void textcolor(int attr, int fg, int bg){
    char command[13];
    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg+30, bg+40);
    printf("%s", command);
    fflush(stdout);
}

int infof(const char *format, ...){
    sysLogStatus = LOG_INFO;
    va_list valist;
    va_start(valist, format);
    textcolor(RESET,  WHITE, RESET_BG);
    printOn(format, valist);
    textcolor(RESET,  WHITE, RESET_BG);
    va_end(valist);
    return 0;
}

int warnf(const char *format, ...){
    sysLogStatus = LOG_WARNING;
    va_list valist;
    va_start(valist, format);
    textcolor(ITALICS, YELLOW, RESET_BG);
    printOn(format, valist);
    textcolor(RESET,  WHITE, RESET_BG);
    va_end(valist);
    return 0;
}

int errorf(const char *format, ...){
    sysLogStatus = LOG_ERR;
    va_list valist;
    va_start(valist, format);
    textcolor(BRIGHT, RED, RESET_BG);
    printOn(format, valist);
    textcolor(RESET,  WHITE, RESET_BG);
    va_end(valist);
    return 0;
}

int panicf(const char *format, ...){
    sysLogStatus = LOG_CRIT;
    va_list valist;
    va_start(valist, format);
    textcolor(BLINK, WHITE, RED);
    printOn(format, valist);
    textcolor(RESET,  WHITE, RESET_BG);
    va_end(valist);
    raise(SIGABRT);
    return 0;
}