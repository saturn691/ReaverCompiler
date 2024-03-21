#include <stdio.h>

int array_basic(int num);

int main() {
    int num = 10;
    int val;
    val = array_basic(num);
    printf("%d\n", val);
    return !(val == 10);
}
