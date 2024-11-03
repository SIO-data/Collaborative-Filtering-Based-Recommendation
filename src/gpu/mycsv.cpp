#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <regex>

#define ROWS 73421      // Define the maximum rows as needed
#define COLS 100        // Define the maximum columns as needed
#define TARGET_SIZE 40000 // Size of the evaluation sample

// Function to read CSV file and populate the data array
int read_csv(const std::string& filename, std::vector<float>& data) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return -1;
    }

    // Skip the first line (header)
    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "Failed to read header line" << std::endl;
        return -1;
    }

    // Read data
    int row = 0;
    while (std::getline(file, line) && row < ROWS) {
        std::stringstream ss(line);
        std::string token;
        int col = 0;

        // Skip the first column
        std::getline(ss, token, ';');

        while (std::getline(ss, token, ';') && col < COLS) {
            float value = std::atof(token.c_str());
            data[row * COLS + col] = value;
            col++;
        }
        row++;
    }

    file.close();
    return 0;
}

int read_target_csv(const std::string& filename, std::vector<int>& user_resto) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return -1;
    }

    std::string line;
    int row = 0;

    std::regex pattern(R"(User(\d+);Restaurant(\d+);)");

    while (std::getline(file, line) && row < 40000) {

        //printf("Reading line: %s\n", line.c_str());

        int userNum, restaurantNum;
        std::smatch matches;

        // Check if the line matches the pattern
        if (std::regex_search(line, matches, pattern)) {
            userNum = std::stoi(matches[1]);          // Convert first match to integer
            restaurantNum = std::stoi(matches[2]);     // Convert second match to integer

            //printf("User: %d, Restaurant: %d\n", userNum, restaurantNum);

            // Store in the user_resto array
            user_resto[row * 2 + 0] = userNum;
            user_resto[row * 2 + 1] = restaurantNum;

            //printf("User: %d, Restaurant: %d\n", user_resto[row * 2 + 0], user_resto[row * 2 + 1]);
        } else {
            std::cerr << "Failed to parse data from line: " << line << std::endl;
        }
        row++;
    }

    file.close();
    return 0;
}

int write_estimated_csv(const std::string& filename, const std::vector<float>& output, const std::vector<int>& user_resto) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return -1;
    }

    for (int row = 0; row < 40000; row++) {
        file << "User" << user_resto[row * 2 + 0] << ";Restaurant" << user_resto[row * 2 + 1] << ";" << output[row] << "\n";
    }

    file.close();
    return 0;
}
