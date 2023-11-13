#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Using {executed file} {name of the shared file} {semaphore name}");
        exit(-1);
    }

    int fd = open(argv[1], O_RDWR, 0644);
    size_t file_size = lseek(fd, 0, SEEK_END);
    char* mapped = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("map failed!");
        exit(-1);
    }

    sem_t* sem = sem_open(argv[2], 0);
    if (sem == SEM_FAILED) {
        perror("Semaphore 2 failed");
        exit(-1);
    }

    sem_wait(sem);

    int j = 0;
    for (int i = 0; mapped[i] != '\0' && mapped[i] != EOF && mapped[i] != '\n'; ++i) {
        if (!(mapped[i] == ' ' && mapped[j - 1] == ' ')) {
            mapped[j++] = mapped[i];
        }
    }
    mapped[j] = '\0';
    
    sem_post(sem);

    // printf("%s\n", mapped);

    sem_close(sem);
    munmap(mapped, file_size);
    close(fd);

    return 0;
}