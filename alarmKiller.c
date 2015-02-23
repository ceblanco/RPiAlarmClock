// Based very heavily on http://www.thelinuxdaily.com/2010/05/grab-raw-keyboard-input-from-event-device-node-devinputevent
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
 
int main (int argc, char *argv[])
{
    struct input_event ev[64];
    int fd, rd, value, last_value, size = sizeof (struct input_event);
    char name[256] = "Unknown";
    //char *device = NULL;
    char device[64] = "/dev/input/event3";
    char killOmxCmd[64] = "pkill omxplayer.bin";
 
    if ((getuid ()) != 0)
        printf ("You are not root! This may not work...\n");
 
    //Open Device
    if ((fd = open (device, O_RDONLY)) == -1)
        printf ("%s is not a vaild device.\n", device);
 
    //Print Device Name
    ioctl (fd, EVIOCGNAME (sizeof (name)), name);
    printf ("Reading From : %s (%s)\n", device, name);
 
    value = 0;
    last_value = -1;
    while (1) {
        if ((rd = read (fd, ev, size * 64)) < size)
            perror_exit ("read()");      
 
        value = ev[0].value;
        if (value == last_value) continue;  // discard repeat button presses
 
        if (value != ' ' && ev[1].value == 1 && ev[1].type == 1){ // Only read the key press event
            printf ("Code[%d]\n", (ev[1].code));
            if (ev[1].code == 28) {
                // Dismiss alarm
                system(killOmxCmd);
            } else if (ev[1].code == 1) {
                // Snooze alarm 
                printf("Snooze alarm - not implemented yet\n");
            }
        }
        last_value = value;
    }
 
    return 0;
}
