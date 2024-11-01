#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include "mycsv.h"

#define ROWS 73421   // Replace with actual row count of your dataset
#define COLS 100     // Replace with actual column count, excluding the first column
#define THREADS 1

float _bar(float *user){
    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < COLS; i++) {
        if (user[i] != 99.0f) {
            sum += user[i];
            count++;
        }
    }
    return sum / count;
}

float *_all_bar(float **data){
    float *bars = (float *)malloc(ROWS * sizeof(float));
    for (int i = 0; i < ROWS; i++) {
        bars[i] = _bar(data[i]);
    }
    return bars;
}

float pearson(float *user1, float *user2, float bar1, float bar2){
    float num = 0.0f;
    float den1 = 0.0f;
    float den2 = 0.0f;
    for (int i = 0; i < COLS; i++) {
        if (user1[i] != 99.0f && user2[i] != 99.0f) {
            num += (user1[i] - bar1) * (user2[i] - bar2);
            den1 += (user1[i] - bar1) * (user1[i] - bar1);
            den2 += (user2[i] - bar2) * (user2[i] - bar2);
        }
    }
    return num / (sqrtf(den1) * sqrtf(den2));
}

float _estimation(int user, int resto, float *bar, float **data){
    float estimation = 0.0f;
    float num = 0;
    float den = 0;
    for (int u = 0; u < ROWS; u++){
        if (u != user && data[u][resto] != 99.0f){
            float _pearson = pearson(data[user], data[u], bar[user], bar[u]);
            if (isnan(_pearson)) {
                _pearson = 0.0f;
            }
            num += (_pearson * powf(fabsf(_pearson), 1.5f) * (data[u][resto] - bar[u]));
            den += (powf(fabsf(_pearson), 2.5f));
        }
    }
    estimation = bar[user] + (num / den);
    return estimation;
}

struct thread_data {
    int thread_id;
    float **data;
    float *output;
    int **user_resto;
    float *bars;
};

void* perform_task(void* arg) {
    struct thread_data *th_data = (struct thread_data *)arg;
    int thread_id = th_data->thread_id;
    float **data = th_data->data;
    float *output = th_data->output;
    int **user_resto = th_data->user_resto;
    float *bars = th_data->bars;

    int start = thread_id * (40000 / THREADS);
    int end = (thread_id + 1) * (40000 / THREADS);
    if (thread_id == THREADS - 1) {
        end = 40000;
    }

    for (int i = start; i < end; i++) {
        int user = user_resto[i][0] - 1;
        int resto = user_resto[i][1] - 1;
        output[i] = _estimation(user, resto, bars, data);
    }

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]) {

    // ------------------------------------------------- //
    // --------------- Memory Allocation --------------- //
    // ------------------------------------------------- //

    printf("Allocating memory for data and user_coef...\n");

    // Allocate memory for data array
    float **data = (float **)malloc(ROWS * sizeof(float *));
    for (int i = 0; i < ROWS; i++) {
        data[i] = (float *)malloc(COLS * sizeof(float));
    }

    int **user_resto = (int **)malloc(40000 * sizeof(int *));
    for (int i = 0; i < 40000; i++) {
        user_resto[i] = (int *)malloc(2 * sizeof(int));
    }

    float *output = malloc(40000 * sizeof(float));
    for (int i = 0; i < 40000; i++) {
        output[i] = 0.0f;
    }

    printf("Memory allocated successfully.\n");

    // ------------------------------------------------- //
    // ----------------- Read CSV file ----------------- //
    // ------------------------------------------------- //

    printf("Reading CSV file...\n");

    if (read_csv("./assets/data/recommendation_dataset.csv", data) != 0) {
        return 1;  // Exit if file read fails
    }

    printf("CSV file read successfully.\n");

    // ------------------------------------------------- //
    // ------------ Read Target CSV file --------------- //
    // ------------------------------------------------- //

    printf("Reading target CSV file...\n");

    if (read_target_csv("./assets/data/template.csv", user_resto) != 0) {
        return 1;  // Exit if file read fails
    }
 
    // ------------------------------------------------- //
    // ------------ Compute User Estimations ----------- //
    // ------------------------------------------------- //

    // Compute all _bar values
    float *bars = _all_bar(data);

    printf("Computing user estimations...\n");

    pthread_t threads[THREADS];
    for (int i = 0; i < THREADS; i++) {
        struct thread_data *th_data = (struct thread_data *)malloc(sizeof(struct thread_data));
        th_data->thread_id = i;
        th_data->data = data;
        th_data->output = output;
        th_data->user_resto = user_resto;
        th_data->bars = bars;
        pthread_create(&threads[i], NULL, perform_task, (void *)th_data);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("User estimations computed successfully.\n");

    // ------------------------------------------------- //
    // ----------------- Write to CSV ------------------ //
    // ------------------------------------------------- //

    printf("Writing to CSV file...\n");

    if (write_estimated_csv("./assets/data/estimated_results.csv", output, user_resto) != 0) {
        return 1;  // Exit if file write fails
    }

    printf("CSV file written successfully.\n");

    // Free allocated memory
    for (int i = 0; i < ROWS; i++) {
        free(data[i]);
    }
    free(data);

    for (int i = 0; i < 40000; i++) {
        free(user_resto[i]);
    }
    free(user_resto);

    free(output);

    return 0;
}
