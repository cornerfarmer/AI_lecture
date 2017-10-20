#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void merge(char s[], int l, int n, int r) {
    static char temp[100];
    for (int i = l; i <= r; i++)
        temp[i - l] = s[i];
    int m = n + 1 - l;
    int i1 = 0;
    int i2 = m;
    int len = r - l;
    
    while (l <= r) {
        if (i1 >= m || (i2 <= len && temp[i1] > temp[i2]))
            s[l++] = temp[i2++];
        else
            s[l++] = temp[i1++]; 
    }
}

void sort(char s[], int l, int r) {
    if (l < r) {
        
        sort(s, l, l + (r - l) / 2);
        sort(s, l + (r - l) / 2 + 1, r);
        merge(s, l, l + (r - l) / 2, r);    

        printf("%.*s\n", r - l + 1, s + l);    
    }
}

int main() {
    char s[100];
    int l;

    scanf("%s", s);
    l = strlen(s);

    sort(s, 0, l - 1);

    //printf("%s\n", s);

    return 0;
}