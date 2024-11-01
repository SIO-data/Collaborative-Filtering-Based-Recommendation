#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#define ROWS 73421   // Replace with actual row count of your dataset
#define COLS 100     // Replace with actual column count, excluding the first column
#define IMPORTANCE 1
#define THRESHOLD 0.07f
#define THREADS 8

struct thread_data {
    int thread_id;
    float **data;
    float *output;
    int **user_resto;
};


// Function to compute how similar two users are
float compute_similarity(float *user1, float *user2) {
    float similarity = 0.0f;
    float distance = 0.0f;
    for (int i = 0; i < COLS; i++) {
        if (user1[i] != 99.0f && user2[i] != 99.0f) {
            distance += (fabsf(user1[i] - user2[i]));
        }
    }
    similarity = 1.0f / (1.0f + (distance));
    return similarity;
}

// Function to compute similarity between a user and all other users
float *compute_all_similarities(float **data, int user) {
    float *similarities = (float *)malloc(ROWS * sizeof(float));
    for (int i = 0; i < ROWS; i++) {
        similarities[i] = compute_similarity(data[user], data[i]);
    }
    return similarities;
}

// Function to compute the estimated rating for a user
float *compute_user_estimation(float **data, int user) {
    
    float *similarities = compute_all_similarities(data, user); // Compute similarities between user and all other users

    float sum_similarities = 0.0f;
    for (int i = 0; i < ROWS; i++) {
        if (similarities[i] > THRESHOLD) {
            sum_similarities += powf(similarities[i], IMPORTANCE);
        }
    }

    float *estimation = (float *)malloc(COLS * sizeof(float));
    for (int i = 0; i < COLS; i++) {
        estimation[i] = 0.0f;
    }
    for (int i = 0; i < ROWS; i++) {
        //if (i != user) {
            for (int j = 0; j < COLS; j++) {
                if (data[i][j] != 99.0f && similarities[i] > THRESHOLD) {
                    estimation[j] += powf(similarities[i],IMPORTANCE) * data[i][j];
                }
            }
        //}
    }
    for (int i = 0; i < COLS; i++) {
        estimation[i] = estimation[i] / sum_similarities;
    }

    free(similarities);

    return estimation;
}

float compute_user_resto_estimation(float **data, int user, int resto){
    float *similarities = compute_all_similarities(data, user);
    float sum_similarities = 0.0f;
    /*for (int i = 0; i < ROWS; i++) {
        if (similarities[i] > THRESHOLD) {
            sum_similarities += powf(similarities[i], IMPORTANCE);
        }
    }

    float estimation = 0.0f;
    for (int i = 0; i < ROWS; i++) {
        if (data[i][resto] != 99.0f && similarities[i] > THRESHOLD) {
            estimation += powf(similarities[i], IMPORTANCE) * data[i][resto];
        }
    }
    estimation = estimation / sum_similarities;*/

    int closest_users = 0;
    float similarity = 0.0f;
    for (int i = 0; i < ROWS; i++) {
        if (similarities[i] > similarity && data[i][resto] != 99.0f) {
            similarity = similarities[i];
            closest_users = i;
        }
    }

    free(similarities);

    float estimation = data[closest_users][resto];

    return estimation;
}

void* perform_task(void* arg) {
    struct thread_data *th_data = (struct thread_data *)arg;
    int thread_id = th_data->thread_id;
    float **data = th_data->data;
    float *output = th_data->output;
    int **user_resto = th_data->user_resto;

    int start = thread_id * (40000 / THREADS);
    int end = (thread_id + 1) * (40000 / THREADS);

    for (int row = start; row < end; row++) {
        int user = user_resto[row][0];
        int resto = user_resto[row][1];
        float estimation = compute_user_resto_estimation(data, user-1, resto-1);
        output[row] = estimation;
    }
    
    free(arg);  // Free the memory allocated for thread ID
    return NULL;
}

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

int main(int argc, char *argv[]) {
    printf("Allocating memory for data and user_coef...\n");

    // Allocate memory for data array
    float **data = (float **)malloc(ROWS * sizeof(float *));
    for (int i = 0; i < ROWS; i++) {
        data[i] = (float *)malloc(COLS * sizeof(float));
    }

    // Allocate memory for estimate_data array
    float **estimate_data = (float **)malloc(ROWS * sizeof(float *));
    for (int i = 0; i < ROWS; i++) {
        estimate_data[i] = (float *)malloc(COLS * sizeof(float));
    }

    int **user_resto = (int **)malloc(40000 * sizeof(int *));
    for (int i = 0; i < 40000; i++) {
        user_resto[i] = (int *)malloc(2 * sizeof(int));
    }

    /*/ Allocate memory for user_coef array
    float ***user_coef = (float ***)malloc(ROWS * sizeof(float **));
    for (int i = 0; i < ROWS; i++) {
        user_coef[i] = (float **)malloc(COLS * sizeof(float *));
        for (int j = 0; j < COLS; j++) {
            user_coef[i][j] = (float *)calloc(COLS, sizeof(float));
        }
    }*/

    printf("Memory allocated successfully.\n");

    printf("Reading CSV file...\n");

    if (read_csv("./assets/data/recommendation_dataset.csv", data) != 0) {
        return 1;  // Exit if file read fails
    }

    printf("CSV file read successfully.\n");

    printf("Reading target CSV file...\n");

    if (read_target_csv("./assets/data/template.csv", user_resto) != 0) {
        return 1;  // Exit if file read fails
    }

    printf("Calculating user coefficients...\n");

    /*/ Iterate over users (rows in dataset)
    for (int user = 0; user < ROWS; user++) {
        for (int i = 0; i < COLS; i++) {
            if (data[user][i] != -1.0f) {  // Skip if value is -1 (indicating missing)
                for (int j = 0; j < COLS; j++) {
                    if (data[user][j] != -1.0f) {
                        if (i == j) {
                            user_coef[user][i][j] = 1.0f;
                        }else if(data[user][i] == 99.0f || data[user][j] == 99.0f) {
                            user_coef[user][i][j] = 0.0f;
                        }
                        else if (data[user][i] == data[user][j]) {
                            user_coef[user][i][j] = 1.0f;
                        } else {
                            user_coef[user][i][j] = 1.0f / (5 * fabsf(data[user][i] - data[user][j]));
                        }
                    }
                }
            }
        }
        //printf("Done with user %d\n", user);
    }*/

    printf("User coefficients calculated successfully.\n");

    printf("Computing user estimations...\n");

    /*pthread_t threads[THREADS];
    for (int i = 0; i < THREADS; i++) {
        int *thread_id = (int *)malloc(sizeof(int));
        *thread_id = i;
        struct thread_data *th_data = (struct thread_data *)malloc(sizeof(struct thread_data));
        th_data->thread_id = i;
        th_data->data = data;
        th_data->estimate_data = estimate_data;
        pthread_create(&threads[i], NULL, perform_task, (void *)th_data);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }*/

    float *output = malloc(40000 * sizeof(float));
   
    pthread_t threads[THREADS];
    for (int i = 0; i < THREADS; i++) {
        struct thread_data *th_data = (struct thread_data *)malloc(sizeof(struct thread_data));
        th_data->thread_id = i;
        th_data->data = data;
        th_data->output = output;
        th_data->user_resto = user_resto;
        pthread_create(&threads[i], NULL, perform_task, (void *)th_data);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("User estimations computed successfully.\n");
    printf("Writing estimated data to CSV file...\n");

    //Open file output.csv
    FILE *file = fopen("./assets/data/estimated_dataset.csv", "w");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    for (int row = 0; row < 40000; row++) {
        fprintf(file, "User%d;Restaurant%d;%f\n", user_resto[row][0], user_resto[row][1], output[row]);
    }

    fclose(file);

    printf("CSV file written successfully.\n");

    // Free allocated memory
    for (int i = 0; i < ROWS; i++) {
        free(data[i]);
    }
    free(data);

    /*for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            free(user_coef[i][j]);
        }
        free(user_coef[i]);
    }
    free(user_coef);
    */

    return 0;
}
