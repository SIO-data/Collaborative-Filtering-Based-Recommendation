#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS 73421      // Define the maximum rows as needed
#define COLS 100       // Define the maximum columns as needed

// Function to read CSV file and populate the data array
int read_csv(const char *filename, float **data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }
    
    // Skip the first line
    char line[2048];
    if (!fgets(line, 2048, file)) { // Check if fgets successfully reads the first line
        perror("Failed to read header line");
        fclose(file);
        return -1;
    }

    //printf("Header skipped: %s\n", line);  // Print the header to verify it's skipped

    int row = 0;
    while (fgets(line, 1024, file) && row < ROWS) {
        int col = 0;
        char *token = strtok(line, ";");
        
        // Skip the first column
        token = strtok(NULL, ";");

        while (token && col < COLS) {
            float value = atof(token);
            data[row][col] = value;
            token = strtok(NULL, ";");
            col++;
        }
        row++;
    }

    fclose(file);
    return 0;
}

int read_target_csv(const char *filename, int **user_resto) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    char line[1024];
    int row = 0;
    int userNum, restaurantNum;

    while (fgets(line, 1024, file) && row < 40000) {
        if (sscanf(line, "User%d;Restaurant%d;", &userNum, &restaurantNum) == 2) {
            user_resto[row][0] = userNum;
            user_resto[row][1] = restaurantNum;
        } else {
            printf("Échec de l'analyse des données de la ligne : %s", line);
        }
        row++;
    }

    fclose(file);
    return 0;
}

int write_estimated_csv(const char *filename, float *output, int **user_resto) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    for (int row = 0; row < 40000; row++) {
        fprintf(file, "User%d;Restaurant%d;%f\n", user_resto[row][0], user_resto[row][1], output[row]);
    }

    fclose(file);

    return 0;
}

// Function to write the data array to a CSV file
int write_csv(const char *filename, float **data) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    fprintf(file, "DATASET");
    for (int i = 0; i < COLS; i++) {
        fprintf(file, ";Restaurant%d", i+1);
    }

    for (int i = 0; i < ROWS; i++) {
        fprintf(file, "User%d;", i+1);
        for (int j = 0; j < COLS; j++) {
            fprintf(file, "%f;", data[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}