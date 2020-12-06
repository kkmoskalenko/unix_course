#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <time.h>

int main() {
    FILE *fp[2];

    if (p2open("sort -n", fp) == -1) { return 1; }

    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        fprintf(fp[0], "%d\n", rand() % 100);
    }

    fclose(fp[0]);

    char c;
    int count = 0;
    while (read(fileno(fp[1]), &c, 1) == 1) {
        if (c == '\n') {
            printf(++count % 10 == 0 ? "\n" : " ");
            continue;
        }

        printf("%c", c);
    }

    p2close(fp);

    return 0;
}
