#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS 73421   // Replace with actual row count of your dataset
#define COLS 100    // Replace with actual column count, excluding the first column

// Function to read CSV file and populate the data array
int read_csv(const char *filename, double data[ROWS][COLS]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }
    
    // Skip the first line
    char line[1024];
    fgets(line, 1024, file);

    int row = 0;
    while (fgets(line, 1024, file) && row < ROWS) {
        int col = 0;
        char *token = strtok(line, ";");
        
        // Skip the first column
        token = strtok(NULL, ";");

        while (token && col < COLS) {
            double value = atof(token);
            data[row][col] = (value == 99.0) ? -1.0 : value;
            token = strtok(NULL, ";");
            col++;
        }
        row++;
    }

    fclose(file);
    return 0;
}

int main() {

    double data[ROWS][COLS];
    double user_coef[ROWS][COLS][COLS] = {0};  // 3D array initialization to 0

    if (read_csv("../assets/data/recommendation_dataset.csv", data) != 0) {
        return 1;  // Exit if file read fails
    }

    // Iterate over users (rows in dataset)
    for (int user = 0; user < ROWS; user++) {
        for (int i = 0; i < COLS; i++) {
            if (data[user][i] != -1) {  // Skip if value is -1 (indicating missing)
                for (int j = 0; j < COLS; j++) {
                    if (data[user][j] != -1) {
                        if (i == j) {
                            user_coef[user][i][j] = 1.0;
                        } else if (data[user][i] == data[user][j]) {
                            user_coef[user][i][j] = 1.0;
                        } else {
                            user_coef[user][i][j] = 1.0 / fabs(data[user][i] - data[user][j]);
                        }
                    }
                }
            }
        }
        printf("Done with user %d\n", user);
    }

    // Print user_coef for the first user
    printf("user_coef[0]:\n");
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%f ", user_coef[0][i][j]);
        }
        printf("\n");
    }

    return 0;
}
