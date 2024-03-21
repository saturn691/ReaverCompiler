int array_mul(int arr1[][2], int arr2[][2]) {
    int result[2][2];
    int i;
    int j;
    int k;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            result[i][j] = 0;
            for (k = 0; k < 2; k++) {
                result[i][j] += arr1[i][k] * arr2[k][j];
            }
        }
    }
    return result[1][1];
}
