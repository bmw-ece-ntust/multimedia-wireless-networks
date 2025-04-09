import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def load_throughput_data():
    # Load data from CSV files
    df_a = pd.read_csv('throughput_ap_a.csv', names=['time', 'throughput'])
    df_b = pd.read_csv('throughput_ap_b.csv', names=['time', 'throughput'])
    
    return df_a, df_b

def plot_throughput(df_a, df_b):
    plt.figure(figsize=(12, 6))
    
    # Plot throughput for both APs
    plt.plot(df_a['time'], df_a['throughput'], label='AP A', color='blue')
    plt.plot(df_b['time'], df_b['throughput'], label='AP B', color='red')
    
    # Add vertical lines for movement events
    plt.axvline(x=180, color='gray', linestyle='--', label='First Movement')
    plt.axvline(x=360, color='gray', linestyle='--', label='Second Movement')
    
    # Customize the plot
    plt.title('WiFi Network Throughput Over Time')
    plt.xlabel('Time (seconds)')
    plt.ylabel('Throughput (Mbps)')
    plt.grid(True)
    plt.legend()
    
    # Save the plot
    plt.savefig('throughput_analysis.png')
    plt.close()

def analyze_movements(df_a, df_b):
    # Time periods for analysis
    periods = [(0, 180), (180, 360), (360, 600)]
    period_names = ['Initial', 'After First Movement', 'After Second Movement']
    
    # Calculate average throughput for each period
    summary = []
    for period, name in zip(periods, period_names):
        start, end = period
        avg_a = df_a[(df_a['time'] >= start) & (df_a['time'] < end)]['throughput'].mean()
        avg_b = df_b[(df_b['time'] >= start) & (df_b['time'] < end)]['throughput'].mean()
        
        summary.append({
            'Period': name,
            'AP A Avg Throughput': avg_a,
            'AP B Avg Throughput': avg_b,
            'Total Throughput': avg_a + avg_b
        })
    
    # Convert to DataFrame and save to file
    summary_df = pd.DataFrame(summary)
    summary_df.to_csv('throughput_summary.txt', index=False)
    
    return summary_df

def main():
    # Load data
    df_a, df_b = load_throughput_data()
    
    # Generate plots
    plot_throughput(df_a, df_b)
    
    # Analyze movements and generate summary
    summary = analyze_movements(df_a, df_b)
    print("\nThroughput Analysis Summary:")
    print(summary.to_string(index=False))

if __name__ == "__main__":
    main()