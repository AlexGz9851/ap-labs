#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include<sys/types.h>
#include<sys/stat.h>

#define REPORT_FILE "packages_report.txt"

#define BUFFER 255

#define LEN_REMOVED   7 
#define LEN_INSTALLED 9 
#define LEN_UPGRADED  8
#define LEN_DATE 16

struct package{
    char* name;
    char* installDate;
    char* lastUpdateDate;
    int updates;
    char* removalDate;
};

int dprintf(int fd, const char *format, ...);
size_t read(int fd, void *buf, size_t cnt);
int close(int fd);
void analizeLog(char *logFile, char *report);
char * getLineStr(int fileD,int *lenLine,int * isEOF);
char *duplicateCharArr(char *line, int * len);
int readChar(int fd);
int findAction(char * line, int *index);
int areEqualStr(char * str1, char * str2, int i1, int i2, int len);
struct package * addPackage(struct package * pkgs, int *currentLen, int *maxLen, char * line,int indx);
int addUpdate(struct package * pkgs, int *currentLen ,char * line,int indx);
int addRemove(struct package * pkgs, int *currentLen, char * line,int indx);
int searchPkg(struct package * pkgs, int * currentLen, char *name);
char * getName(char * line, int nameStart);
struct package * duplicatePkgsArr(struct package * pkgs, int *maxLen, int * currentLen);
void printAnalysis(int fd, struct package * pkgs, int len);
void printSummary(int fd, int installed,int upgraded, int removed);
void printLine(char * line, int lenLine);
int compareStrings(char * s1, char * s2);

int main(int argc, char **argv) {

    if (argc < 2) {
	printf("Usage:./pacman-analizer.o pacman.log\n");
	return 1;
    } 

    analizeLog(argv[1], REPORT_FILE);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    int lenArrPackages, lenCurrPackages;
    lenArrPackages=100;
    lenCurrPackages=0;
    struct package * pkgs = (struct package *) malloc(lenArrPackages * sizeof(struct package));

    printf("Generating Report from: [%s] log file\n", logFile);

    // Open log File
    int logFileD = open(logFile, O_RDONLY);
    if(logFileD < 0){
        printf("Error while opening log file. Abort.\n");
        return;
    }

    
    //Start analysis
    int lenLine, isEOF, pkgsInstalled,pkgsUpgraded, pkgsRemoved;
    char *line;
    lenLine =pkgsInstalled=pkgsRemoved= pkgsUpgraded =isEOF =0;
    for(int x=0;;x++){
        line= getLineStr(logFileD, &lenLine, &isEOF);
        //printLine(line, lenLine);
        int action, indexAction, err; 
        action = indexAction =0;
        action = findAction(line, &indexAction);
        if(action==LEN_UPGRADED){   
            err = addUpdate(pkgs,  &lenCurrPackages,   line, indexAction);
        }
        struct package * newPackArray;
        if(action==LEN_INSTALLED){
            newPackArray = addPackage(pkgs,  &lenCurrPackages,  &lenArrPackages, line, indexAction);
            if(newPackArray != NULL) {
                pkgsInstalled++;
                pkgs = newPackArray;
            }
            
        }
        if(action==LEN_REMOVED){
            err = addRemove(pkgs,  &lenCurrPackages, line, indexAction);
            if(!err) pkgsRemoved++;
        }
            
        free(line);
        if(isEOF) break;
    }

    //Close log File
    if(close(logFileD) < 0){
        printf("Error while closing log file. Abort.\n");
        return;
    }

    for(int i=0;i<lenCurrPackages;i++){
        if(pkgs[i].removalDate[0] == '-'){ //Counting just updates of not removed packages.
            pkgsUpgraded += pkgs[i].updates;
        }
    }
    

    // Open report file
    int reportFD = open(report, O_WRONLY | O_CREAT | O_TRUNC,  S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if(reportFD < 0){
        printf("Error while opening report.\n");
        //If can't open report file, send report to terminal. Go on.
    }

    //print summary and details, either on reportFile or terminal.
    printSummary(reportFD, pkgsInstalled, pkgsUpgraded, pkgsRemoved);
    printAnalysis(reportFD, pkgs, lenCurrPackages);

    //Close log File
    if(close(reportFD) < 0){
        printf("Error while closing report file. Abort.\n");
        return;
    }
    //Final Instructions.
    printf("Report is generated at: [%s]\n", reportFD < 0? "To terminal.":report);
}

void printSummary(int fd, int installed,int upgraded, int removed){
    if(fd < 0) fd = 1; // si no hay fd, manda a stdout (1)
    dprintf(fd, "Pacman Packages Report\n");
    dprintf(fd, "----------------------\n");
    dprintf(fd, "- Installed packages : %d\n",installed);
    dprintf(fd, "- Removed packages   : %d\n",removed);
    dprintf(fd, "- Upgraded packages  : %d\n",upgraded);
    dprintf(fd, "- Current installed  : %d\n",installed-removed);
}

void printAnalysis(int fd, struct package * pkgs, int len){
    if(fd < 0) fd = 1; // si no hay fd, manda a stdout (1)
    dprintf(fd, "List of packages\n");
    dprintf(fd, "----------------\n\n");
    for(int i = 0 ;i<len; i++){
        dprintf(fd, "- Package Name      : %s\n",pkgs[i].name);
        dprintf(fd, "- Install date      : %s\n",pkgs[i].installDate);
        dprintf(fd, "- Last update date  : %s\n",pkgs[i].lastUpdateDate);
        dprintf(fd, "- How many updates  : %d\n",pkgs[i].updates);
        dprintf(fd, "- Removal date      : %s\n\n",pkgs[i].removalDate);
    }
}

void printLine(char * line, int lenLine){
    for(int i=0;i<lenLine;i++){
        printf("%c",line[i]);
    }printf("\n");
}

char * getLineStr(int fileD,int *lenLine,int * isEOF){
    int ch, cnt, buffLen;
    cnt = 0;
    buffLen =BUFFER;
    char * lineBuff= (char *) malloc(BUFFER * sizeof(char *));
    ch = readChar(fileD);
    while(ch!= EOF && ch!= '\n'){
        lineBuff[cnt] =(char)ch;
        cnt++;
        if(cnt==buffLen) duplicateCharArr(lineBuff, &buffLen);
        ch = readChar(fileD);
    }
    lineBuff[cnt] ='\0';
    cnt++;

    (*isEOF) = (ch==EOF);
    (*lenLine )= cnt;

    char * finalLine = (char * ) malloc((*lenLine ) *sizeof(char *));
    memcpy(finalLine,lineBuff, sizeof(char)*(*lenLine ));
    return finalLine;
}

char * duplicateCharArr(char *line, int * len){
    char * result =(char *) malloc(2 * (*len) * sizeof(char *));
    memcpy(result,line , sizeof(char)*(*len ));
    *len = (*len)<<1;
    free(line);
    return result;
}

struct package * duplicatePkgsArr(struct package * pkgs, int *maxLen, int *currentLen){
    struct package * result =(struct package *) malloc(2 * (*maxLen) * sizeof(struct package));
    for(int i=0;i<(*currentLen);i++){
        result[i].installDate = pkgs[i].installDate;
        result[i].lastUpdateDate = pkgs[i].lastUpdateDate;
        result[i].removalDate= pkgs[i].removalDate;
        result[i].name= pkgs[i].name;
        result[i].updates= pkgs[i].updates;

        pkgs[i].installDate=NULL;
        pkgs[i].lastUpdateDate=NULL;
        pkgs[i].removalDate=NULL;
        pkgs[i].name=NULL;

    }

    *maxLen *= 2;
    free(pkgs);
    return result;
}

int readChar(int fd){
    char buffer[1];
    if(read(fd, buffer, 1)) return buffer[0];
    return EOF;
}

int findAction(char * line, int *index){
    char removed[LEN_REMOVED] ={'r','e','m','o','v','e','d'}; 
    char installed[LEN_INSTALLED] ={'i','n','s','t','a','l','l','e','d'}; 
    char upgraded[LEN_UPGRADED] ={'u','p','g','r','a','d','e','d'}; 
    int countClSqBr = 0;
    int i=0;
    if(line[i]!='[') return 0; 
    while(countClSqBr < 2 ){
        if(line[i]==']') countClSqBr++;
        i++;
    }
    i++;//space
    *index =i;
    if(areEqualStr(installed, line, 0, i, LEN_INSTALLED)) return LEN_INSTALLED;
    if(areEqualStr(upgraded, line, 0, i, LEN_UPGRADED)) return LEN_UPGRADED;
    if(areEqualStr(removed, line, 0, i, LEN_REMOVED)) return LEN_REMOVED;
    return 0;
    
}

int areEqualStr(char * str1, char * str2, int i1, int i2, int len){
    for(int j=0;j<len;j++){
        if(str1[j+i1]!=str2[j+i2])  return 0;  
    }
    return 1;
}

struct package * addPackage(struct package *pkgs, int *currentLen, int *maxLen, char * line,int indx){
    int lenName, nameStart;
    char * installDate, *lastDate, *name, *rdate;
    //name
    nameStart = indx +LEN_INSTALLED +1;
    name = getName(line, nameStart);
    rdate = (char *)malloc((2)*(sizeof(char)));
    rdate[0]='-';
    rdate[1]='\0';

    //dates
    installDate= (char *)malloc((LEN_DATE+1)*(sizeof(char)));
    lastDate= (char *)malloc((LEN_DATE+1)*(sizeof(char)));
    

    for(int i=0;i<LEN_DATE;i++){
        installDate[i] = line[1+i];
        lastDate[i] = line[1+i];
    }
    installDate[LEN_DATE] ='\0';
    lastDate[LEN_DATE] ='\0';
    
    //assign
    if((*currentLen) ==( *maxLen)){
        pkgs  = duplicatePkgsArr(pkgs, maxLen,currentLen);
        if(pkgs == NULL) return NULL;
    }
    pkgs[*currentLen].installDate= installDate;
    pkgs[*currentLen].lastUpdateDate= lastDate;
    pkgs[*currentLen].removalDate =rdate;
    pkgs[*currentLen].name = name;
    pkgs[*currentLen].updates = 0;
    //printf("%s\n", pkgs[*currentLen].name);
    (*currentLen)++;
    return pkgs;
}

char * getName(char * line, int nameStart){
    char * name;
    int lenName=0;
    while(line[lenName+nameStart]!=' '){
        lenName++;
    }
    name = (char *)malloc((lenName+1)*(sizeof(char)));
    for(int k=0;k<lenName;k++){
        name[k] = line[k+nameStart];
    }
    name[lenName] = '\0';
    return name;
}

int addUpdate(struct package * pkgs, int *currentLen,  char * line,int indx){
    char  *lastDate;
    int nameStart = indx +LEN_UPGRADED +1;
    //dates
    
    lastDate= (char *)malloc((LEN_DATE+1)*(sizeof(char)));
    for(int i=0;i<LEN_DATE;i++){
        lastDate[i] = line[1+i];
    }
    lastDate[LEN_DATE] ='\0';
    //assign
    char * name = getName(line,nameStart );
    int  i = searchPkg(pkgs, currentLen, name);
    if(i<0) return 1;
    free(pkgs[i].lastUpdateDate);
    pkgs[i].lastUpdateDate= lastDate;
    pkgs[i].updates ++;
    return 0;
}

int addRemove(struct package * pkgs, int *currentLen, char * line,int indx){
    int nameStart = indx +LEN_REMOVED +1;
    char  *lastDate;
    char * name = getName(line, nameStart);
    int i = searchPkg(pkgs, currentLen,name);

    //dates
    
    lastDate= (char *)malloc((LEN_DATE+1)*(sizeof(char)));
    for(int i=0;i<LEN_DATE;i++){
        lastDate[i] = line[1+i];
    }
    lastDate[LEN_DATE] ='\0';

    //assign
    if(i<0) return 1;
    pkgs[i].removalDate = lastDate;
    return 0;
}

int searchPkg(struct package * pkgs, int *currentLen, char * name){

    //start searching the newest package with that name.
    //If we installed - removed - reinstalled some package,
    //We'll have 2 logs with that name. The newest is the current.
    int equal;
    for(int i=(*currentLen)-1;i>=0;i--){
        char * pn = pkgs[i].name;
        equal=compareStrings(pn, name);
        if(equal==0) return i;
    }
    return -1;
}

int compareStrings(char * s1, char * s2){
    int i=0;
    while(s1[i] !='\0' && s2[i] !='\0'){
        if(s1[i] < s2[i]) return -1;
        if(s1[i] > s2[i]) return 1;
        i++;
    }
    if(s2[i] !='\0')  return -1;
    if(s1[i] !='\0' ) return 1;
    return 0;
}

