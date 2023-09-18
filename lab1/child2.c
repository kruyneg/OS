#include <stdio.h>

int main() {
    char s[100];

    s[0] = getchar();
    int i = 1;
    do {
        char c = getchar();
        if (!(c == ' ' && s[i - 1] == ' ')) {
            s[i++] = c;
        }
    } while (s[i - 1] != 0 && s[i - 1] != '\n');
    s[i] = 0;

    printf("%s\n", s);

    return 0;
}