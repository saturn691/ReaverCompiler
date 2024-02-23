#include <stdio.h>

int dotProduct(int arr1[], int arr2[], int size);

int main() {
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5, 6};
    int size = sizeof(arr1) / sizeof(arr1[0]);

    // int arr1[] = {1, 2, 3, 4, 5, 6};
    // int arr2[] = {4, 5, 6, 7, 8, 9};
    // int size = sizeof(arr1) / sizeof(arr1[0]);

    int dot = dotProduct(arr1, arr2, size);

    printf("Size: %d\n", size);
    printf("Dot product: %d\n", dot);

    return !(dot == 32);
}
