MAE: 3.053 (using 100 trees and 6 in deep)


model = XGBRegressor(n_estimators=100, learning_rate=0.1, max_depth=6, random_state=42)

**Table of MAE - XGBoost:**

| Parameters | MAE |
| -------- | ------- |
| 100 trees & 6 deep | 3.053 |
| 200 trees & 12 deep | 3.134 |
