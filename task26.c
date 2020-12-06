#include <stdio.h>

int main() {
    char line[BUFSIZ];

    FILE *fpin = popen("echo Hello, World!", "r");
    FILE *fpout = popen("tr [:lower:] [:upper:]", "w");

    while (fgets(line, BUFSIZ, fpin) != NULL) {
        fputs(line, fpout);
    }

    pclose(fpin);
    pclose(fpout);
}
