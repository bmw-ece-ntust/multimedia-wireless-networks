import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('ue_positions.csv')
time_points = sorted(df['Time'].unique())

for t in time_points:
    plt.figure(figsize=(12, 6))
    subset = df[df['Time'] == t]
    plt.scatter(subset['X'], subset['Y'], c='blue', label=f'Time={t}')

    # 每 2 個 UE 標一個，讓畫面不擠
    for _, row in subset.iterrows():
        if row['UE_Index'] % 2 == 0:
            plt.text(row['X'] + 2, row['Y'] + 2, str(row['UE_Index']), fontsize=8)

    plt.title(f'UE positions at Time={t}')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.grid(True)
    plt.legend()
    plt.axis('equal')
    plt.tight_layout()
    plt.savefig(f'ue_positions_time_{t}.png')
    plt.show()
