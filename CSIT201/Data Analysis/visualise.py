import pandas as pd
import matplotlib.pyplot as plt

# Load the data
data = pd.read_csv("match_data.csv")

# Plotting
plt.figure(figsize=(14, 7))
for outcome, group in data.groupby('Outcome'):
    plt.scatter(group['MatchID'], group['AttackPoints'] - group['DefensePoints'], label=f'{outcome}', s=100)

plt.axhline(0, color='red', linestyle='--') # A line at zero to indicate the balance point between Attack and Defense
plt.xlabel('Match ID')
plt.ylabel('Attack Points minus Defense Points')
plt.title('Match by Match Comparison of Attack vs. Defense')
plt.legend()
plt.grid(True)
plt.show()