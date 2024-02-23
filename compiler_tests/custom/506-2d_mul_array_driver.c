#include <stdio.h>

#define N 3

int array_mul(int arr1[][N], int arr2[][N], int size);

int main() {
    int arr1[N][N] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    int arr2[N][N] = {{9, 8, 7}, {6, 5, 4}, {3, 2, 1}};
    int result[N][N];

    array_mul(arr1, arr2, N);

    printf("Result:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }

    return 0;
}
