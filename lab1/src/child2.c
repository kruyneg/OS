#include <stdio.h>
#include <unistd.h>

int main() {
    char s[100];

    read(STDIN_FILENO, s, sizeof(char));
    int i = 1;
    do {
        char c;
        read(STDIN_FILENO, &c, sizeof(char));
        if (!(c == ' ' && s[i - 1] == ' ')) {
            s[i++] = c;
        }
    } while (s[i - 1] != 0 && s[i - 1] != '\n' && s[i - 1] != EOF);
    s[i] = 0;

    write(STDOUT_FILENO, s, sizeof(char) * i);

    return 0;
}