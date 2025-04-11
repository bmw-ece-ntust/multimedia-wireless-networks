import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# get csv
ap1 = pd.read_csv("ap1_throughput.csv")
ap2 = pd.read_csv("ap2_throughput.csv")

# setting axis and plot
plt.figure(figsize=(10, 6))
plt.plot(ap1['Time'], ap1['Throughput(Mbps)'], label='AP1')
plt.plot(ap2['Time'], ap2['Throughput(Mbps)'], label='AP2')
plt.xlabel('Time (s)')
plt.ylabel('Throughput (Mbps)')
plt.title('Per-AP Throughput Over Time')
plt.legend()
plt.grid(True)

# Adjust scale
plt.xticks(np.arange(0, 31, 1))  # 每 1 秒
plt.yticks(np.arange(0, ap1['Throughput(Mbps)'].max() + 0.5, 0.2))  # 每 0.2 Mbps

# store in png
plt.savefig("APs_throughput_plot.png")
plt.close()


# get csv
sta_count = pd.read_csv("ap_sta_count.csv")

# setting axis and plot
plt.figure(figsize=(10, 6))
plt.plot(sta_count['Time'], sta_count['AP1_STA_Count'], label='AP1 Connected STAs')
plt.plot(sta_count['Time'], sta_count['AP2_STA_Count'], label='AP2 Connected STAs')
plt.xlabel('Time (s)')
plt.ylabel('Number of Connected STAs')
plt.title('Number of Connected STAs Over Time')
plt.legend()
plt.grid(True)

# Adjust scale
plt.xticks(np.arange(0, 31, 1))  # 每秒
plt.yticks(np.arange(0, sta_count[['AP1_STA_Count', 'AP2_STA_Count']].values.max() + 1, 2))  # 每 2 個 STA

# store in png
plt.savefig("APs_sta_count_plot.png")
plt.close()
