#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/termios.h>
#include <aio.h>
#include <errno.h>

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    char host[] = "loripsum.net";
    int port = 80;

    // Create a socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket error");
        exit(-1);
    }

    // Lookup the IP address
    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        perror("invalid remote hostname");
        exit(-1);
    }

    // Open the socket
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    memcpy(&address.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    if (connect(fd, (struct sockaddr *) &address, sizeof(address)) == -1) {
        perror("connect error");
        exit(-1);
    }

    // Schedule the read request
    static struct aiocb read_request;
    char buf[100];

    memset(&read_request, 0, sizeof(read_request));
    read_request.aio_fildes = fd;
    read_request.aio_buf = buf;
    read_request.aio_nbytes = sizeof(buf);
    if (aio_read(&read_request) == -1) {
        perror("aio_read error");
        exit(-1);
    }

    // Send the request
    char message[] =
            "GET /api/100/short HTTP/1.0\r\n"
            "Host: loripsum.net\r\n\r\n";
    int message_bytes = sizeof(message) - 1;
    if (write(fd, message, message_bytes) != message_bytes) {
        perror("write error");
        exit(-1);
    }

    // Receive the response
    enableRawMode();

    while (1) {
        static const struct aiocb *list[2] = {&read_request, NULL};
        aio_suspend(list, 1, NULL);
        int size = aio_return(&read_request);
        if (size > 0) {
            write(1, buf, size);
            aio_read(&read_request);
        } else if (size == 0) {
            break;
        } else if (errno != EINPROGRESS) {
            perror("reading from socket");
        }
    }
}
