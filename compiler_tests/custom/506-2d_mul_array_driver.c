#include <stdio.h>

int array_mul(int arr1[][2], int arr2[][2]);

// For reference: declaration of 'arr1' and 'arr2' as multidimensional arrays
// must have bounds for all dimensions except the first

int main() {
    int arr1[2][2] = {{1, 2}, {3, 4}};
    int arr2[2][2] = {{5, 6}, {7, 8}};

    int res = array_mul(arr1, arr2);

    return !(res == 50);

}
