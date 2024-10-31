import pandas as pd

input_file = 'recommendation_dataset.csv'

dfp = pd.read_csv(input_file, sep=';', skiprows=1)
dfp = dfp.iloc[:, 1:].replace(99.0, None)

print(dfp)
mean = dfp.mean(axis=None, skipna=True)

dfp = dfp.fillna(mean)
print(dfp)

