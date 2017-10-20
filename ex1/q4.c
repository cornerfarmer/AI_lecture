#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main() {
    char s[100];
    int l;
    char elem;

    scanf("%s", s);
    l = strlen(s);

    for (int i = 1; i < l; i++) {
        elem = s[i];
        int j = i;
        while (j > 0 && s[j - 1] > elem) {
            s[j] = s[j - 1];
            j--;
        }

        s[j] = elem;
        printf("%s\n", s);
    }

    if (l == 1)
        printf("%s\n", s);

    return 0;
}