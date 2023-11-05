#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>


void create_two_process(int* a, int* b) {
    *a = fork();
    if (*a > 0) {
        *b = fork();
    }
    if (*a == -1 || *b == -1) {
        perror("fork error");
        exit(-1);
    }
    return;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Using {executed file} {name of the shared file}");
        exit(-1);
    }

    int parent_pid = getpid(), pid1, pid2;
    
    // opening of the mapped file
    int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("can't open file");
        exit(-1);
    }
    ftruncate(fd, 1024);
    size_t file_size = lseek(fd, 0, SEEK_END);
    char* mapped = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("map failed!");
        exit(-1);
    }

    // Reading of the string s

    int i = 0;
    do {
        char c;
        c = getchar();
        mapped[i] = c;
        ++i;
    } while (mapped[i - 1] != EOF && mapped[i - 1] != '\n');

    // creating a semaphore

    sem_t* sem = sem_open("/sem_lab3", O_CREAT, 0777, 0);
    if (sem ==SEM_FAILED) {
        perror("Semaphore failed");
        exit(-1);
    }
    sem_post(sem);

    create_two_process(&pid1, &pid2); // create child1 & child2

    if (pid1 > 0 && pid2 > 0) { // Parent process
        waitpid(pid1, NULL, WUNTRACED);
        waitpid(pid2, NULL, WUNTRACED);

        ftruncate(fd, strlen(mapped));
        printf("%s\n", mapped);
    }
    else if (pid1 == 0) { // Child1 process
        execl("./child1", "./child1", argv[1], "/sem_lab3");
    }
    else { // Child2 process
        execl("./child2", "./child2", argv[1], "/sem_lab3");
    }

    sem_close(sem);
    sem_unlink("/sem_lab3");

    printf("The result was written to %s\n", argv[1]);

    munmap(mapped, strlen(mapped));
    close(fd);

    return 0;
}