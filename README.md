## Pearson Correlation Coefficient Method

In the branch of the repository, we have implemented the Pearson Correlation Coefficient method to find the similarity between two users and then predict the rating of a restaurant by a user.

### Running the code

The code is written in C to get the best performance. Every mathematical operation is done with basics mathematical operations, so the code is very fast. To run the code, you need to have `gcc` installed in your system with POSIX libraries (Linux and MacOS) or MinGW custom installation (Windows). To compile the code, you can use the following command:

```bash
make THREADS=4
```

This command will compile the code with 4 threads. You can change the number of threads as you want. To run the code, you can use the following command:

```bash
./bin/main
```

This command will run the code and show the results in the terminal. The source CSV files are in the `assets/data` folder. You can change the source files in the `main.c` file.

> **CPU**: As the set of data is large, the code may take some time to run depending on the number of threads you are using, and your system's performance. With 12 threads, the code takes more than 5 minutes to run.

> **Memory**: The code doesn't use a lot of memory. It should run on any system.

Output can be found at `output/estimated_results.csv`. You can change the output file using the `OUTPUT_FOLDER` macro in the make command.
eg. 
```bash
make THREADS=4 OUTPUT_FOLDER=another_output_folder
```

### Results

The results are stored in the `output/estimated_results.csv` file. The file contains the following columns:

- `user_id`: The user ID of the user.
- `restaurant_id`: The restaurant ID of the restaurant.
- `rating`: The estimated rating of the user for the restaurant.

To compute the MAE of this method, you can use the following command:

```bash
python ./assets/data/compute_mae.py ./assets/data/target_recommendations.csv ./output/estimated_results.csv
``` 

This command will show a MAE of `3.341` for the given data.

