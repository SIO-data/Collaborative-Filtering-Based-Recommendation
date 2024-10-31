import pandas as pd

input_file = '../assets/recommendation_dataset.csv'

df = df[:, 1:]

dfp = pd.read_csv(input_file, sep=';', skiprows=1)
dfp = dfp.iloc[:, 1:].replace(99.0, None)

print(dfp)
print(dfp.mean(axis=None, skipna=True))