#include <unistd.h>
#include <stdio.h>
#include <termios.h>

int main() {
    struct termios tty, savetty;

    tcgetattr(STDIN_FILENO, &tty);
    savetty = tty;

    tty.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);

    printf("Enter a symbol!\n");

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        printf("You entered: %c\n", c);
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &savetty);

    return 0;
}
