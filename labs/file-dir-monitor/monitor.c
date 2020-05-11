#include <stdio.h>
#include <errno.h>
#include "logger.h"
#include <sys/inotify.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>

#define WAIT_WRITE 2
#define WRITE_ACTION 1
#define NO_WRITE 0
static void handle_events(int fd, int * wd, int argc, char *argv[]);

char renamed_file[4096];
char rename_file_status=WRITE_ACTION;

static void handle_events(int fd, int * wd, int argc, char *argv[]){
	char _Alignas(struct inotify_event) buf[4096];
	const struct inotify_event *event;
	size_t len;
	char *ptr;

	
	/* Loop while events can be read from inotify file descriptor. */
	
	for (;;) {
		/* Read some events. */
		
		len = read(fd, buf, sizeof buf);
		if (len == -1 && errno != EAGAIN) {
			errorf("There was some error trying to read the inotify events.\n");
			return;
		}
		/* If the nonblocking read() found no events to read, then 
		it returns -1 with errno set to EAGAIN. In that case,
		we exit the loop. */

		if (len <= 0)
			break;

		/* Loop over all events in the buffer */

		for (ptr = buf; ptr < buf + len; 
			ptr += sizeof(struct inotify_event) + event->len) {
		
			event = (const struct inotify_event *) ptr;

			/* Print event type */
            //infof("operation> %d\n", event->mask);
			if (event->mask & IN_CREATE){
				//TODO LOG in create FILE.
				infof("Created: \n");
				rename_file_status=WRITE_ACTION;
			}else if (event->mask & IN_DELETE){
				//TODO LOG in delete  FILE.
				infof("Deleted: \n");
				rename_file_status=WRITE_ACTION;
			}else if (event->mask & IN_MOVED_FROM){
				//TODO LOG TO rename file FILE.
				rename_file_status=WAIT_WRITE;
				strcpy(renamed_file, event->name);
			}else if (event->mask & IN_MOVED_TO){
				//TODO LOG TO rename file FILE.
				if(rename_file_status==WAIT_WRITE){
					if(strlen(renamed_file)!=NO_WRITE){
						infof("Renamed from: %s to: \n", renamed_file);
					}else{
						infof("Renamed to: \n");
					}
					renamed_file[0]=0;
					rename_file_status=WRITE_ACTION;
				}else{
					rename_file_status=NO_WRITE;
					continue;
				}
			}else{
                rename_file_status=NO_WRITE;
            }
			if(rename_file_status==WRITE_ACTION){
				/* Print the name of the watched directory */
				for (int i = 1; i < argc; ++i) {
					if (wd[i] == event->wd) {
						infof("%s/", argv[i]);
						break;
					}
	    			}
		
				/* Print the name of the file */
				if (event->len)
					infof("%s", event->name);
				
				/* Print type of filesystem object */
		
				if (event->mask & IN_ISDIR)
					infof(" [directory]\n");
				else	
					infof(" [file]\n");
			}

		}
	}
}
int main(int argc, char ** argv){
	
	char buf;
	int fd, poll_num;
	int *wd;
	nfds_t nfds;
	struct pollfd fds[2];

	renamed_file[0]=0;
	if(argc!=2){
		errorf("Number of attributes incorrect. Aborting.\n");
		return -1;		
	}
    infof("codes: increate %d indelete %d inmovedfrom %d inmovedto %d\n",IN_CREATE ,IN_DELETE , IN_MOVED_FROM,IN_MOVED_TO);

	fd = inotify_init1(IN_NONBLOCK);
	if (fd == -1) {
		errorf("inotify_init1");
		return -1;	
	}
	
	/* Allocate memory for watch descriptors */

	wd = calloc(argc, sizeof(int));
	if (wd == NULL) {
		errorf("calloc");
		return -1;
	}

	/* Mark directories for events
		- file was opened
		- file was closed */


	for (int i = 1; i < argc; i++) {
		wd[i] = inotify_add_watch(fd, argv[i],
					IN_OPEN | IN_CLOSE);
		if (wd[i] == -1) {
			errorf( "Cannot watch '%s': %s\n", argv[i], strerror(errno));
			return -1;
		}
	}

	/* Prepare for polling */

	nfds = 2;

	/* Console input */

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;

	/* Inotify input */

	fds[1].fd = fd;
	fds[1].events = POLLIN;

	/* Wait for events and/or terminal input */

	warnf("Listening for events.\n");
	while (1) {
		poll_num = poll(fds, nfds, -1);
		if (poll_num == -1) {
			if (errno == EINTR)
				continue;
			errorf("poll");
			return -1;
		}

		if (poll_num > 0) {
			if (fds[0].revents & POLLIN) {
				/* Console input is available. Empty stdin and quit */
				while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
					continue;
				break;
			}

			if (fds[1].revents & POLLIN) {
				/* Inotify events are available */
				handle_events(fd, wd, argc, argv);
			}
		}
	}

	warnf("Listening for events stopped.\n");

	/* Close inotify file descriptor */

	close(fd);
	free(wd);
	return 0;
	
}

