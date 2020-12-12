#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

char *socket_path = "./socket32";

int main() {
    char buf[100];
    int fd, rc;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("connect error");
        exit(-1);
    }

    while ((rc = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if (write(fd, buf, rc) != rc) {
            if (rc > 0) {
                fprintf(stderr, "partial write");
            } else {
                perror("write error");
                exit(-1);
            }
        }
    }

    return 0;
}
