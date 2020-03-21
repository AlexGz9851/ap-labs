#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER 255

size_t read (int fd, void* buf, size_t cnt);

void filecopy(int ifp, int ofp)
{
    char stream[BUFFER];
    int cnt;
    while( (cnt = read(ifp, stream, BUFFER)) > 0){
        if( write(ofp, stream, cnt) != cnt) write(2, "Error while writing\n", 21);
    }
}

/* cat:  concatenate files, version 2 */
int main(int argc, char *argv[])
{
    int fp;
    char *prog = argv[0];   /* program name for errors */

    if (argc == 1)  /* no args; copy standard input */
        filecopy(0, 1);
    else
        while (--argc > 0)
            if ((fp = open(*++argv, O_RDONLY)) == -1) {
                write(2, prog, strlen(prog));
                write(2, ": can't open ", 13);
                write(2,*argv, strlen(*argv));
                write(2,"\n",1);
                return 1;
            } else {
                filecopy(fp, 1);
                close(fp);
            }

    if (ferror(stdout)) {
        write(2,prog, strlen(prog));
        write(2,": error writing stdout\n", 24);
        fprintf(stderr, "%s: ", prog);
        return 2;
    }

    return 0;
}
