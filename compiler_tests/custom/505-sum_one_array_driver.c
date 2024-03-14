#include <stdio.h>

int sumOneArray(int arr1[], int size);

int main() {
    int arr1[] = {1, 2, 3, 4};
    int size = sizeof(arr1) / sizeof(arr1[0]);

    int sum = sumOneArray(arr1, size);

    printf("Size: %d\n", size);
    printf("Sum: %d\n", sum);

    return !(sum == 10);
}
