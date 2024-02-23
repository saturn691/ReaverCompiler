int array_mul(int arr1[][N], int arr2[][N], int size) {
    int result[size][size];
    int i, j, k;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            result[i][j] = 0;
            for (k = 0; k < size; k++) {
                result[i][j] += arr1[i][k] * arr2[k][j];
            }
        }
    }
}
