#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

extern char **environ;

int main(int argc, char *argv[]) {
    char options[] = ":ispuU:cC:dvV:";  /* valid options */
    int c;

    while ((c = getopt(argc, argv, options)) != EOF) {
        switch (c) {
            // Печатает реальные и эффективные идентификаторы пользователя и группы
            case 'i': {
                printf("uid: %d, euid: %d, gid: %d, egid: %d\n",
                       getuid(), geteuid(), getgid(), getegid());
                break;
            }

                // Процесс становится лидером группы
            case 's': {
                if (-1 == setpgid(0, 0)) {
                    perror("failed to set the group leader process\n");
                } else {
                    printf("the group leader process has been set successfully\n");
                }
                break;
            }

                // Печатает идентификаторы процесса, процесса-родителя и группы процессов
            case 'p': {
                printf("pid: %d, ppid: %d, pgrp: %d\n",
                       getpid(), getppid(), getpgrp());
                break;
            }

                // Печатает значение ulimit
            case 'u': {
                struct rlimit rlp;
                if (-1 == getrlimit(RLIMIT_FSIZE, &rlp)) {
                    perror("failed to get ulimit\n");
                } else {
                    printf("ulimit value: %llu\n", rlp.rlim_max);
                }
                break;
            }

                // Изменяет значение ulimit
            case 'U': {
                long new_ulimit = strtol(optarg, NULL, 10);

                if (0 == new_ulimit) {
                    perror("invalid argument for the -U option\n");
                    break;
                }

                struct rlimit rlp;
                if (-1 == getrlimit(RLIMIT_FSIZE, &rlp)) {
                    perror("failed to get the ulimit value\n");
                    break;
                }
                rlp.rlim_cur = new_ulimit;
                if (-1 == setrlimit(RLIMIT_FSIZE, &rlp)) {
                    perror("failed to set the ulimit value\n");
                } else {
                    printf("the ulimit value has been set successfully\n");
                }
                break;
            }

                // Печатает размер в байтах core-файла, который может быть создан.
            case 'c': {
                struct rlimit rlp;
                if (-1 == getrlimit(RLIMIT_CORE, &rlp)) {
                    perror("failed to get the core-file cap limit\n");
                } else {
                    printf("core-file cap limit: %llu\n", rlp.rlim_max);
                }
                break;
            }

                // Изменяет размер core-файла
            case 'C': {
                long long new_rlim = strtoll(optarg, NULL, 10);

                if (0 == new_rlim) {
                    perror("invalid argument for the -C option\n");
                    break;
                }

                struct rlimit rlp;
                if (-1 == getrlimit(RLIMIT_CORE, &rlp)) {
                    perror("failed to get the core-file cap limit\n");
                    break;
                }
                rlp.rlim_cur = new_rlim;
                if (-1 == setrlimit(RLIMIT_CORE, &rlp)) {
                    perror("failed to set the core-file cap limit\n");
                } else {
                    printf("the core-file cap limit has been set successfully\n");
                }
                break;
            }

                // Печатает текущую рабочую директорию
            case 'd': {
                char *pathname = getenv("PWD");

                if (NULL == pathname) {
                    perror("failed to get the current directory\n");
                } else {
                    printf("current directory: %s\n", pathname);
                }
                break;
            }

                // Распечатывает переменные среды и их значения
            case 'v': {
                char **ptr = environ;
                for (; *ptr != NULL; ptr++) {
                    printf("%s\n", *ptr);
                }
                break;
            }

                // Вносит новую переменную в среду или изменяет значение существующей переменной.
            case 'V': {
                if (-1 == putenv(optarg)) {
                    printf("failed to set the environmental variable");
                }
                break;
            }

            case ':': {
                printf("missing argument for option: %c\n", optopt);
                break;
            }

            case '?':
            default:
                printf("invalid option: %c\n", optopt);
        }
    }

    return 0;
}
