import pandas as pd
import matplotlib.pyplot as plt

# Load the data
df = pd.read_csv('throughput.csv')  # assuming you saved the data in a CSV file

# Plot
plt.figure(figsize=(14, 8))
plt.plot(df['Time'], df['Throughput_EnbA'], label='EnbA', color='blue', linewidth=2, linestyle='--')
plt.plot(df['Time'], df['Throughput_EnbB'], label='EnbB', color='green', linewidth=2, linestyle='-')
plt.xlabel('Time (s)', fontsize=14)
plt.ylabel('Throughput (Mbps)', fontsize=14)
plt.title('Throughput Over Time: EnbA vs EnbB', fontsize=18, fontweight='bold')
plt.legend(fontsize=12, loc='upper left')
plt.grid(alpha=0.5)
plt.tight_layout()
plt.savefig('throughput.png', dpi=300)
plt.show()
