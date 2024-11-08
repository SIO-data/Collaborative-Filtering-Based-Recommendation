## Neural Networks and Deep Learning Methods

In the branch of the repository, we have implemented a collaborative filtering algorithm using neural networks and deep learning methods. The code is implemented in Python.

### Requirements

The code is implemented in Python 3.11.9 and may not work with other versions of Python. To install the required packages, you can use the following command:

```bash
pip install -r requirements.txt
```

or

```bash
pip3.11 install -r requirements.txt
```

> Note: Be sure to use the correct version of Python and of all the packages to avoid any issues.

### Running the code

To run the code, you can use the following command open the notebook:

```bash
jupyter notebook ./src/main.ipynb
```

> Note: The code use as input the a converted version of the data. To convert the data, you can use the following command:

```bash
make convert_data
./bin/converter ./assets/data/recommendation_dataset.csv ./assets/data/converted.csv
```

> Note: This program is written in C and requires a C compiler with POSIX support. We recommend using the GCC compiler on a Unix-based system.



### Results

The results are stored in the `assets/data/output_neural_network.csv` file. The file contains the following columns:

- `user_id`: The user ID of the user.
- `restaurant_id`: The restaurant ID of the restaurant.
- `rating`: The estimated rating of the user for the restaurant.

To compute the MAE of this method, you can use the following command:

```bash
python ./assets/data/compute_mae.py ./assets/data/target_recommendations.csv ./assets/data/output_neural_network.csv
``` 

This command will show the MAE of the method.
The MAE will not be the same every time you run the code, as the neural network is trained randomly.