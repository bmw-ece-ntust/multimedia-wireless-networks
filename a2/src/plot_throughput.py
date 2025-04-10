import pandas as pd
import matplotlib.pyplot as plt

# Read the data
df = pd.read_csv('throughput.csv')

# Create the plot
plt.figure(figsize=(12, 6))
plt.plot(df['Time'], df['ThroughputA_Mbps'], label='AP A', color='blue')
plt.plot(df['Time'], df['ThroughputB_Mbps'], label='AP B', color='red')
plt.plot(df['Time'], df['ThroughputA_Mbps'] + df['ThroughputB_Mbps'], 
         label='Total', color='green', linestyle='--')

# Add vertical lines for migrations
plt.axvline(x=60, color='gray', linestyle='--', label='First Migration')
plt.axvline(x=120, color='black', linestyle='--', label='Second Migration')

plt.title('Network Throughput Over Time')
plt.xlabel('Time (seconds)')
plt.ylabel('Throughput (Mbps)')
plt.grid(True, alpha=0.3)
plt.legend()
plt.savefig('throughput_analysis.png')
plt.close()