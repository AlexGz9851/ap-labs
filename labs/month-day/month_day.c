#include <stdio.h>
#include <stdlib.h>


void month_day(int year,int yearday, int *pmonth, int *pday);
int isLeap(int year);


int isLeap(int year){ 
    if(year %400 == 0 ) return 1;
    if(year % 100 == 0) return 0;
    if(year % 4 == 0 ) return 1;
    return 0;
}


void month_day(int year,int yearday, int *pmonth, int *pday){
    // 1 jan, 2 feb . . . 12 dec
    int leap = isLeap(year);
    if(yearday<1 || 
    ((yearday>365 && !leap) || (yearday>366 && leap))){
            *pmonth = 0;
            *pday = yearday;
            return;
    }

    int days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    
    int done = 0;
    for(int i=0;i<12;i++){
        if(done) break;
        if(yearday>days[i]+(i==1?leap:0)){
            yearday -= days[i]+(i==1?leap:0);
        }else{
            *pmonth = i+1;
            *pday = yearday;
            done = 1;
        }
    }
}


int main(int argc, char ** argv ) {
    int *pmonth, *pday, day, month;
    day= month = 0;
    pday = &day;
    pmonth = &month;
    char * months[13] ={"Invalid month","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    int year =atoi(argv[1]);
    int yday = atoi(argv[2]);
    month_day(year, yday, pmonth , pday);
    printf("%s %d, %d\n", months[*pmonth], *pday,year);
}