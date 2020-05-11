#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <sys/inotify.h>
#include <string.h>
#include "logger.h"
#include "hashtable.h"

#define BUF_SIZE    ( 1024 * ( ( sizeof (struct inotify_event) ) + 16 ) )


int addFile(const char *fpath, const struct stat *stats, int typeflag);
int handleEvent(struct inotify_event *event);

int inotify;
char* fileRenamed[2];

int handleEvent(struct inotify_event *event){
    char *path = getValue(event->wd); 
    if(event->mask & IN_CREATE && event->mask & IN_ISDIR) {
        char fileName[strlen(path) + strlen(event->name) + 1];
        infof("Dir created: \"%s/%s\"\n", path, event->name);
        strcpy(fileName, path);
        strcat(fileName, "/");
        strcat(fileName, event->name);
        addFile(fileName, NULL, FTW_D);
    }else if (event->mask & IN_CREATE) {
        infof("File created: \"%s/%s\"\n", path, event->name);
    }else if (event->mask & IN_DELETE ) {
        infof("File deleted: \"%s/%s\"\n", path, event->name);
        if(event->mask & IN_ISDIR){ inotify_rm_watch(inotify, event->wd); }
    }else if (event->mask & IN_MOVED_FROM) {
        fileRenamed[0] = event->name;
        fileRenamed[1] = path;
    }else if (event->mask & IN_MOVED_TO) {
        if(strcmp(fileRenamed[1], path)==0){
            infof("File \"%s/%s\" was renamed to: \"%s/%s\"\n", path, fileRenamed[0] , path, event->name);
        }
        fileRenamed[0] = NULL;
        fileRenamed[1] = NULL;
    }
    return 0;
}

int addFile(const char *fpath, const struct stat *stats, int typeflag){
    if(typeflag == FTW_D){
        int wd = inotify_add_watch( inotify, fpath, IN_CREATE | IN_DELETE | IN_MOVE);
        addElement(wd, fpath);
    }
    return 0; 
}


int main(int argc, char **argv){
    if(argc != 2){
        errorf("Error: Needed path as attribute. Aborted.\n");
        return 1;
    }
    inotify = inotify_init();
    if(ftw(argv[1], addFile, 20 )== -1){
        errorf("Error trying to monitor the directory. Aborted.\n");
        return 1;
    }
    
    char buffer[BUF_SIZE];
    warnf("Listening to path: %s\n", argv[1]);
    for(;;){
        int len = read(inotify, buffer, BUF_SIZE);
        for(int i=0; i < len; ){
            struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];     
            handleEvent(event);
            i += ( sizeof (struct inotify_event) ) + event->len;
        }
    }
    return 0;
}
