#include<stdio.h>

int main() {
    int i;
    while(scanf("%d", &i) != EOF)
        printf("%d\n", i * 2);
    return 0;
}