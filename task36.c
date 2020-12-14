#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <sys/termios.h>

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
    struct pollfd poll_fds[2];
    poll_fds[0].fd = STDIN_FILENO;
    poll_fds[0].events = POLLIN;
    poll_fds[1].fd = fd;
    poll_fds[1].events = POLLIN;

    int rc;
    char buf[100];

    int printed_lines = 0;
    int paused = 0;

    enableRawMode();

    while (1) {
        if ((poll(poll_fds, 2, -1)) == -1) {
            perror("poll error");
            exit(-1);
        }

        short stdin_events = poll_fds[0].revents;
        short socket_events = poll_fds[1].revents;

        if (stdin_events & POLLIN) {
            if (paused && getchar() == ' ') {
                printed_lines = 0;
                paused = 0;
            }
        }

        if (socket_events & POLLIN) {
            if (printed_lines < 25) {
                if ((rc = read(fd, buf, sizeof(buf))) > 0) {
                    for (int j = 0; j < rc; j++) {
                        if (buf[j] == '\n') printed_lines++;
                        printf("%c", buf[j]);
                    }
                }

                if (rc == -1) {
                    perror("read");
                    exit(-1);
                } else if (rc == 0) {
                    printf("EOF reached, closing connection\n");
                    close(fd);

                    exit(0);
                }
            } else if (!paused) {
                paused = 1;
                printf("\n\nPress space to scroll down...\n");
            }
        }
    }
}
