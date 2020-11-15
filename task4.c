#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Don't use this constant
#define MAX_STRING_LENGTH 10000

typedef struct Node_s {
    char *string;
    struct Node_s *next;
} Node;

Node *head, *tail;

void init() {
    head = malloc(sizeof(Node));

    head->string = NULL;
    head->next = NULL;

    tail = head;
}

void push(char *string) {
    unsigned long len = strlen(string) + 1;
    char *copyPtr = malloc(len * sizeof(char));
    strcpy(copyPtr, string);

    tail->string = copyPtr;
    tail->next = malloc(sizeof(Node));

    tail = tail->next;
}

void printList() {
    Node *ptr = head;
    while (ptr != NULL) {
        if (ptr->string) {
            printf("%s", ptr->string);
        }
        ptr = ptr->next;
    }
}

int main() {
    char inputBuf[MAX_STRING_LENGTH] = {0};

    init();

    while (fgets(inputBuf, MAX_STRING_LENGTH, stdin)) {
        if (inputBuf[0] == '.') {
            printList();
            return 0;
        }

        push(inputBuf);
    }

    return 0;
}
