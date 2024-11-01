#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS 73421      // Define the maximum rows as needed
#define COLS 100       // Define the maximum columns as needed

// Function to read a CSV file and populate a 2D array of floats
int read_csv(const char *filename, float **data);

// Function to read a CSV file and populate a 2D array of integers representing user and restaurant pairs
int read_target_csv(const char *filename, int **user_resto);

// Function to write a CSV file with estimated results
int write_estimated_csv(const char *filename, float *output, int **user_resto);

// Function to write the data array to a CSV file
int write_csv(const char *filename, float **data);

#endif // CSV_UTILS_H
