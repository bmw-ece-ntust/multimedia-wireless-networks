import matplotlib.pyplot as plt
import pandas as pd

# Load the data from the file
df = pd.read_csv('aggregated_throughput.txt')

# Create the plot
plt.figure(figsize=(10, 6))  # Adjust figure size for better readability
plt.plot(df['Time_s'], df['Throughput_AP_A_Mbps'], label='Throughput_AP_A_Mbps')
plt.plot(df['Time_s'], df['Throughput_AP_B_Mbps'], label='Throughput_AP_B_Mbps')

# Add labels and title
plt.xlabel('Time (s)')
plt.ylabel('Throughput (Mbps)')
plt.title('Throughput over Time for AP A and AP B')

# Add legend to distinguish the lines
plt.legend()

# Add grid for better readability
plt.grid(True)

# Show the plot
plt.show()