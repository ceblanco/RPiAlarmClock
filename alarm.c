// Event handling based very heavily on http://www.thelinuxdaily.com/2010/05/grab-raw-keyboard-input-from-event-device-node-devinputevent
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio_ext.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
 
void handler (int sig)
{
    printf ("\nexiting...(%d)\n", sig);
    exit (0);
}
 
void perror_exit (char *error)
{
    perror (error);
    handler (9);
}

int main(int argc, char **argv) {
    pid_t pid, pid1;
    char dest[100];
    char dPtr;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork failed");
        return -1;
    } else if (pid == 0) {  // child
        // Let's run the media player here
        pid1 = getpid();
        if (argc == 1) {
            execl("/usr/bin/omxplayer", "omxplayer", "-o", "local", "http://172.239.76.148:8850", (char *)0);
        }
        else if (argc == 2) {
            execl("/usr/bin/omxplayer", "omxplayer", "-o", "local", argv[1], (char *)0);
        }
        //Sample classical stream: http://173.239.76.148:8850
        //Sample worldmusic stream: http://65.60.1.98:9034
    } else {    // parent
        // Let's watch for events here
        pid1 = getpid();

        struct input_event ev[64];
        int fd, rd, value, last_value, size = sizeof (struct input_event);
        char name[256] = "Unknown";
        char device[64] = "/dev/input/event3";
        char killOmxCmd[64] = "pkill omxplayer.bin";
 
        if ((getuid ()) != 0)
            printf ("You are not root! This may not work...\n");
            fflush(stdout);
 
        //Open Device
        if ((fd = open (device, O_RDONLY)) == -1)
            printf ("%s is not a vaild device.\n", device);
        FILE *fp = fdopen(fd, "r");
 
        //Print Device Name
        ioctl (fd, EVIOCGNAME (sizeof (name)), name);
 
        value = 0;
        //last_value = -1;
        while (1) {
            if ((rd = read (fd, ev, size * 64)) < size)
                perror_exit ("read()");      
 
            value = ev[0].value;
            //if (value == last_value) continue;
 
            if (value != ' ' && ev[1].value == 1 && ev[1].type == 1){ // Only read the key press event
                printf ("Code[%d]\n", (ev[1].code));
                if (ev[1].code == 28) {
                    // Dismiss alarm
                    printf("Got event 28 - firing off killOmxCmd\n");
                    system(killOmxCmd);
                    // None of the below seem to be working
                    // Try reopening
                    //if ((fd = open (device, O_RDONLY)) == -1)
                    //    printf ("%s is not a vaild device.\n", device);
                    //fp = fdopen(fd, "r");
                    // TODO clear out the input stream with fpurge?
                    //while ((c = getchar()) != XX && c != EOF)
                    // or 
                    //__fpurge(fp);
                } else if (ev[1].code == 1) {
                    // Snooze alarm 
                    //printf("Snooze alarm - not implemented yet\n");
                    // TODO clear out the input stream with fpurge?
                    //if ((fd = open (device, O_RDONLY)) == -1)
                        //printf ("%s is not a vaild device.\n", device);
                    //fp = fdopen(fd, "r");
                }
            }
            //last_value = value;
        }
    }

    return 0;
}
