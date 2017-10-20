#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main() {
    char s[100];
    int l;
    char temp;

    scanf("%s", s);
    l = strlen(s);

    bool swapped;
    bool swappedOnce = false;
    do {
        swapped = false;

        for (int i = 0; i < l - 1; i++) {
            if (s[i] > s[i + 1]) {
                temp = s[i];
                s[i] = s[i + 1];
                s[i + 1] = temp;
                swapped = true;
                swappedOnce = true;
            }
        }

        l--;

        if (swapped)
            printf("%s\n", s);
    } while (swapped);

    if (!swappedOnce)
        printf("%s\n", s);
    return 0;
}