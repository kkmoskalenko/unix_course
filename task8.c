#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char filename[FILENAME_MAX];
    int fd;

    printf("Enter the filename: ");
    scanf("%s", filename);

    if ((fd = open(filename, O_RDWR)) == -1) {
        perror("open error");
        exit(1);
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        fcntl(fd, F_GETLK, &lock);
        printf("The file is already locked (by pid: %d)\n", lock.l_pid);
    } else {
        char command[FILENAME_MAX + 4] = "vim ";
        strcat(command, filename);
        system(command);
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    close(fd);
    return 0;
}
