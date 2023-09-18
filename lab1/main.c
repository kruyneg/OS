#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void create_two_process(int* a, int* b) {
    *a = fork();
    if (*a > 0) {
        *b = fork();
    }
    if (*a == -1 || *b == -1) {
        perror("fork error\n");
        exit(-1);
    }
    return;
}

int main(int argc, char *argv[]) {
    int parent_pid = getpid(), pid1, pid2;
    
    int fd1[2]; // parent -> child1
    if (pipe(fd1) == -1) {
        perror("pipe wasn't created\n");
    }
    int fd2[2]; // child1 -> child2
    if (pipe(fd2) == -1) {
        perror("pipe wasn't created\n");
    }
    int fd3[2]; // child2 -> parent
    if (pipe(fd3) == -1) {
        perror("pipe wasn't created\n");
    }

    char s[100];
    // Reading of the string s
    int i = 0;
    do {
        s[i] = getchar();
        ++i;
    } while (s[i - 1] != 0 && s[i - 1] != '\n');

    create_two_process(&pid1, &pid2); // create child1 & child2

    if (pid1 > 0 && pid2 > 0) { // Parent process
        size_t s_size = strlen(s);

        close(fd1[0]);
        write(fd1[1], s, s_size); // passes the string to the Child1 process
        close(fd1[1]);

        waitpid(pid1, NULL, WUNTRACED);
        waitpid(pid2, NULL, WUNTRACED);

        char res[100] = "";
        close(fd3[1]);
        read(fd3[0], res, s_size); // gets the result from the Child2 process

        for (int i = 0; i < s_size || res[i] == 0; ++i) { // puts '\0' at the end of res
            if (res[i] == '\n') {
                res[i] = 0;
                break;
            }
        }

        printf("Answer: %s\n", res);
        close(fd3[0]);
    }
    else if (pid1 == 0) { // Child1 process
        close(fd1[1]);
        dup2(fd1[0], STDIN_FILENO);

        close(fd2[0]);
        dup2(fd2[1], STDOUT_FILENO);

        execvp("./child1", argv);
    }
    else { // Child2 process
        close(fd2[1]);
        dup2(fd2[0], STDIN_FILENO);

        close(fd3[0]);
        dup2(fd3[1], STDOUT_FILENO);

        execvp("./child2", argv);
    }

    return 0;
}