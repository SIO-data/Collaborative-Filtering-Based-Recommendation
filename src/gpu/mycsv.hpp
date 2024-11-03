#ifndef CSV_UTILS_HPP
#define CSV_UTILS_HPP

#include <vector>
#include <string>
#include <regex>

// Define maximum rows and columns
#define ROWS 73421      // Define the maximum rows as needed
#define COLS 100        // Define the maximum columns as needed
#define TARGET_SIZE 40000 // Size of the evaluation sample

// Function to read CSV file and populate the data array
int read_csv(const std::string& filename, std::vector<float>& data);

// Function to read target CSV file and populate user_restaurant pairs
int read_target_csv(const std::string& filename, std::vector<int>& user_resto);

// Function to write estimated results to a CSV file
int write_estimated_csv(const std::string& filename, const std::vector<float>& output, const std::vector<int>& user_resto);

#endif // CSV_UTILS_HPP
