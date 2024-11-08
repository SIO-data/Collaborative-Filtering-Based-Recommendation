#include "mycsv.h"

int main(int argc, char *argv[]){

    printf("Starting conversion of CSV file\n");

    float **data = (float **)malloc(ROWS * sizeof(float *));
    for (int i = 0; i < ROWS; i++) {
        data[i] = (float *)malloc(COLS * sizeof(float));
    }

    if (read_csv(argv[1], data) != 0) {
        printf("Failed to read CSV file\n");
        return -1;
    }

    FILE *file = fopen(argv[2], "w");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    fprintf(file, "user;restaurant;rating\n");

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (data[i][j] == 99) continue;
            fprintf(file, "%d;%d;%f\n", i+1, j+1, data[i][j]);
        }
    }

    fclose(file);

    printf("Conversion done\n");

    return 0;
}