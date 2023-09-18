#include <stdio.h>
#include <ctype.h>

int main() {
    char s[100];

    int i = 0;
    do {
        char c = getchar();
        s[i++] = tolower(c);
    } while (s[i - 1] != 0 && s[i - 1] != '\n');
    s[i] = 0;

    printf("%s\n", s);

    return 0;
}
