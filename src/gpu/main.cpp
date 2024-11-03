#include <sycl/sycl.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "mycsv.hpp"

#define ROWS 73421       // Number of rows in your dataset
#define COLS 100         // Number of columns (excluding the first column)
#define TARGET_SIZE 40000 // Size of the evaluation sample

using namespace sycl;

std::vector<float> debug_vector(100, -99.0f);

// Function to calculate the average values for each user (excluding the value 99.0f)
std::vector<float> _all_bar(const std::vector<float>& data) {
    std::vector<float> bars(ROWS, 0.0f);

    // Create a SYCL queue to use the GPU

    printf("Creating a SYCL queue...\n");

    sycl::queue q;

    {

        // Create buffers from vectors
        sycl::buffer<float, 1> data_buf(data.data(), range<1> {ROWS * COLS});
        sycl::buffer<float, 1> bars_buf(bars.data(), range<1> {ROWS});

        q.submit([&](handler &h){
            auto data = data_buf.get_access<access::mode::read>(h);
            auto bars = bars_buf.get_access<access::mode::write>(h);

            h.parallel_for(range<1>(ROWS), [=](id<1> i) {
                float sum = 0.0f;
                int count = 0;
                for (int j = 0; j < COLS; j++) {
                    if (data[i * COLS + j] != 99.0f) {
                        sum += data[i * COLS + j];
                        count++;
                    }
                }
                bars[i] = (count > 0) ? sum / count : 0.0f;
            }); 
        });
    }

    printf("Done with the SYCL queue\n");

    return bars;
}

// Function to estimate user ratings for a restaurant using Pearson correlations
std::vector<float> _estimation_parallel(const std::vector<float>& data,
                                        const std::vector<int>& user_resto,
                                        const std::vector<float>& bars) {
    std::vector<float> output(TARGET_SIZE, 0.0f);

    queue q;

    {

        // Debug buffer
        sycl::buffer<float, 1> debug_buf(debug_vector.data(), range<1> {100});

        // Create buffers from vectors
        sycl::buffer<float, 1>    data_buf(       data.data(),        range<1>{ROWS * COLS});
        sycl::buffer<int, 1>      user_resto_buf( user_resto.data(),  range<1>{TARGET_SIZE * 2});
        sycl::buffer<float, 1>    bars_buf(       bars.data(),        range<1>{ROWS});
        sycl::buffer<float, 1>    output_buf(     output.data(),      range<1>{TARGET_SIZE});

        q.submit([&](handler &h){
            auto data           = data_buf.get_access<access::mode::read>(h);
            auto user_resto     = user_resto_buf.get_access<access::mode::read>(h);
            auto bars           = bars_buf.get_access<access::mode::read>(h);
            auto output         = output_buf.get_access<access::mode::write>(h);

            auto debug = debug_buf.get_access<access::mode::write>(h);

            h.parallel_for(range<1>(TARGET_SIZE), [=](id<1> i){

                if (i < 100) {
                    debug[i] = static_cast<float>(i);
                }

                int user = user_resto[i * 2 + 0] - 1;
                int resto = user_resto[i * 2 + 1] - 1;

                float num = 0.0f;
                float den = 0.0f;
                

                for (int u = 0; u < ROWS; u++) {

                    if (u == user || data[u * COLS + resto] == 99.0f)
                        continue;

                    float pearson = 0.0f;
                    float num1 = 0.0f;
                    float den1 = 0.0f;
                    float den2 = 0.0f;

                    // calcul de pearson
                    for (int r = 0; r < COLS; r++){

                        if (data[user * COLS + r] == 99.f || data[u * COLS + r] == 99.f)
                            continue;

                        num1 += (data[user * COLS + r] - bars[user]) * (data[u * COLS + r] - bars[u]);
                        den1 += (data[user * COLS + r] - bars[user]) * (data[user * COLS + r] - bars[user]);
                        den2 += (data[u * COLS + r] - bars[u]) * (data[u * COLS + r] - bars[u]);
                    }

                    pearson = (den1 > 0 && den2 > 0) ? num1 / (sycl::sqrt(den1) * sycl::sqrt(den2)) : 0.0f;

                    num += pearson * (data[u * COLS + resto] - bars[u]);
                    den += sycl::fabs(pearson);
                }

                output[i] = (den > 0) ? bars[user] + (num / den) : bars[user];
                
            }); 
        });

        q.wait_and_throw();

    }

    return output;
}

// Main function
int main(int argc, char *argv[]) {

    if (argc && argv) {} // To avoid compilation warnings   

    printf("Reading data...\n");

    // Allocate data using std::vector
    std::vector<float> data(ROWS * COLS, 0.0f);

    // Allocate user_resto using std::vector
    std::vector<int> user_resto(TARGET_SIZE * 2, 0);

    // Read the data and targets from CSV files
    if (read_csv("../../assets/data/recommendation_dataset.csv", data) != 0) {
        perror("Error reading CSV file");
        return 1;
    }

    printf("Reading target data...\n");

    if (read_target_csv("../../assets/data/template.csv", user_resto) != 0) {
        perror("Error reading target CSV file");
        return 1;
    }

    printf("Calculating average values...\n");

    // Calculate average values for each user
    std::vector<float> bars = _all_bar(data); 

    printf("Estimating user ratings...\n");

    // Parallel estimation
    std::vector<float> output = _estimation_parallel(data, user_resto, bars);

    for (int i = 0; i < 10; i++) {
        printf("Estimated rating for user %d and restaurant %d: %f\n", user_resto[i * 2 + 0], user_resto[i * 2 + 1], output[i]);
    }

    printf("Printing debug vector...\n");
    for (int i = 0; i < 100; i++) {
        printf("%f ", debug_vector[i]);
    }

    printf("Writing results...\n");

    // Write the result to a CSV file
    if (write_estimated_csv("../../assets/data/estimated_results.csv", output, user_resto) != 0) {
        return 1;
    }

    printf("Done!\n");

    return 0; // No memory cleanup needed as vectors handle it
}
