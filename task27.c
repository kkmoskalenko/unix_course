#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) { return 1; }
    char *filename = argv[1];

    char line[BUFSIZ];

    strcat(line, "grep '^$' ");
    strcat(line, filename);

    FILE *fpin = popen(line, "r");
    FILE *fpout = popen("wc -l", "w");

    while (fgets(line, BUFSIZ, fpin) != NULL) {
        fputs(line, fpout);
    }

    pclose(fpin);
    pclose(fpout);
}
