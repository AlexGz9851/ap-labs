#include <stdio.h>
#include <stdarg.h>

#define RESET 0
#define BRIGHT 1
#define DIM 2
#define ITALICS 3 //This are the codes my virtual machine thrown.
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

void textcolor(int attr, int fg, int bg){
    char command[13];

    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg+30, bg+40);
    printf("%s", command);
}

int infof(const char *format, ...){
    va_list valist;
    va_start(valist, format);
    textcolor(RESET,  WHITE, RESET_BG);
    vprintf(format, valist);
    textcolor(RESET,  WHITE, RESET_BG);
    va_end(valist);

}

int warnf(const char *format, ...){
    va_list valist;
    va_start(valist, format);
    textcolor(ITALICS, YELLOW, RESET_BG);
    vprintf(format, valist);
    textcolor(RESET,  WHITE, RESET_BG);
    va_end(valist);
}

int errorf(const char *format, ...){
    va_list valist;
    va_start(valist, format);
    textcolor(BRIGHT, RED, RESET_BG);
    vprintf(format, valist);
    textcolor(RESET,  WHITE, RESET_BG);
    va_end(valist);
}

int panicf(const char *format, ...){
    va_list valist;
    va_start(valist, format);
    textcolor(BLINK, WHITE, RED);
    vprintf(format, valist);
    textcolor(RESET,  WHITE, RESET_BG);
    va_end(valist);
}
