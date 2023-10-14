#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

int main() {
    char s[100];

    int i = 0;
    do {
        char c;
        read(STDIN_FILENO, &c, sizeof(char));
        s[i++] = tolower(c);
    } while (s[i - 1] != 0 && s[i - 1] != '\n' && s[i - 1] != EOF);
    s[i] = 0;

    i = 0;
    do {
        write(STDOUT_FILENO, &s[i++], sizeof(char));
    } while (s[i - 1] != 0 && s[i - 1] != '\n');

    return 0;
}
