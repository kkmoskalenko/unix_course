#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

char *socket_path = "./socket30";

int main() {
    char buf[100];
    int fd, cl, rc;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    unlink(socket_path);

    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind error");
        exit(-1);
    }

    if (listen(fd, 5) == -1) {
        perror("listen error");
        exit(-1);
    }

    while (1) {
        if ((cl = accept(fd, NULL, NULL)) == -1) {
            perror("accept error");
            continue;
        }

        while ((rc = read(cl, buf, sizeof(buf))) > 0) {
            for (int i = 0; i < rc; i++) {
                buf[i] = (char) toupper(buf[i]);
                printf("%c", buf[i]);
            }
        }

        if (rc == -1) {
            perror("read");
            exit(-1);
        } else if (rc == 0) {
            printf("EOF reached, closing connection\n");
            close(cl);
        }
    }
}
