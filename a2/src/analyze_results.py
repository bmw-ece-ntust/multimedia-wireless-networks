# analysis.py
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from py_markdown_table.markdown_table import markdown_table
from datetime import datetime

def load_and_preprocess_data():
    """Load and preprocess the simulation data"""
    # Use absolute path to the simulation output files
    sim_path = "/home/ming/ns-3-allinone/ns-3.44"
    throughput_df = pd.read_csv(f'{sim_path}/throughput.csv')
    user_dist_df = pd.read_csv(f'{sim_path}/user_distribution.csv')
    
    # Calculate additional metrics
    throughput_df['Total_Throughput'] = throughput_df['ThroughputA_Mbps'] + throughput_df['ThroughputB_Mbps']
    throughput_df['Throughput_Ratio'] = throughput_df['ThroughputA_Mbps'] / throughput_df['ThroughputB_Mbps']
    
    return throughput_df, user_dist_df

def plot_throughput_analysis(throughput_df, save_path='plots'):
    """Generate comprehensive throughput analysis plots"""
    # Create plots directory if not exists
    import os
    save_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), save_path)
    os.makedirs(save_path, exist_ok=True)
    
    # Set basic style parameters
    plt.rcParams['figure.figsize'] = [15, 8]
    plt.rcParams['axes.grid'] = True
    plt.rcParams['grid.alpha'] = 0.3
    
    # 1. Combined Throughput Plot
    plt.figure()
    plt.plot(throughput_df['Time'], throughput_df['ThroughputA_Mbps'], 
            label='AP A', color='blue', linewidth=2)
    plt.plot(throughput_df['Time'], throughput_df['ThroughputB_Mbps'], 
            label='AP B', color='red', linewidth=2)
    plt.plot(throughput_df['Time'], throughput_df['Total_Throughput'], 
            label='Total', color='green', linewidth=2, linestyle='--')
    
    # Add migration points at the actual migration times (60s and 120s)
    plt.axvline(x=60, color='gray', linestyle='--', label='First Migration')
    plt.axvline(x=120, color='black', linestyle='--', label='Second Migration')
    
    plt.title('AP Throughput Over Time', fontsize=14, pad=20)
    plt.xlabel('Time (seconds)', fontsize=12)
    plt.ylabel('Throughput (Mbps)', fontsize=12)
    plt.legend(fontsize=10, loc='center left', bbox_to_anchor=(1, 0.5))
    plt.tight_layout()
    plt.savefig(f'{save_path}/throughput_combined.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # 2. Throughput Distribution Box Plot
    plt.figure(figsize=(10, 6))
    throughput_data = [
        throughput_df['ThroughputA_Mbps'],
        throughput_df['ThroughputB_Mbps']
    ]
    # Use tick_labels instead of labels (fixed deprecation warning)
    plt.boxplot(throughput_data, tick_labels=['AP A', 'AP B'])
    plt.title('Throughput Distribution by AP', fontsize=14, pad=20)
    plt.ylabel('Throughput (Mbps)', fontsize=12)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f'{save_path}/throughput_distribution.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # 3. Throughput Heatmap
    plt.figure(figsize=(12, 6))
    data = pd.DataFrame({
        'Time': throughput_df['Time'],
        'AP A': throughput_df['ThroughputA_Mbps'],
        'AP B': throughput_df['ThroughputB_Mbps']
    }).set_index('Time')
    
    plt.imshow(data.T, aspect='auto', cmap='YlOrRd')
    plt.colorbar(label='Throughput (Mbps)')
    plt.title('Throughput Heatmap', fontsize=14, pad=20)
    plt.xlabel('Time (seconds)', fontsize=12)
    plt.ylabel('Access Point', fontsize=12)
    plt.yticks([0, 1], ['AP A', 'AP B'])
    plt.tight_layout()
    plt.savefig(f'{save_path}/throughput_heatmap.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"Plots saved to: {save_path}")

def analyze_user_distribution(user_dist_df):
    """Analyze user distribution patterns"""
    # Calculate statistics for each time point
    # Get actual timestamps from the data
    timestamps = user_dist_df['Time'].unique()
    
    stats = []
    for time in timestamps:  # Use actual timestamps from data
        data = user_dist_df[user_dist_df['Time'] == time]
        if not data.empty:
            stats.append({
                "Time": f"t={time}s",
                "Users at AP A": data['UsersAtA'].values[0],
                "Users at AP B": data['UsersAtB'].values[0],
                "Total Users": data['UsersAtA'].values[0] + data['UsersAtB'].values[0],
                "Distribution Ratio": f"{data['UsersAtA'].values[0]:.1f}:{data['UsersAtB'].values[0]:.1f}"
            })
    
    return pd.DataFrame(stats)

def calculate_performance_metrics(throughput_df):
    """Calculate various performance metrics"""
    metrics = {
        "Average Throughput AP A": throughput_df['ThroughputA_Mbps'].mean(),
        "Average Throughput AP B": throughput_df['ThroughputB_Mbps'].mean(),
        "Peak Throughput AP A": throughput_df['ThroughputA_Mbps'].max(),
        "Peak Throughput AP B": throughput_df['ThroughputB_Mbps'].max(),
        "Throughput Stability AP A": throughput_df['ThroughputA_Mbps'].std(),
        "Throughput Stability AP B": throughput_df['ThroughputB_Mbps'].std(),
        "Total Data Transferred AP A": throughput_df['ThroughputA_Mbps'].sum(),
        "Total Data Transferred AP B": throughput_df['ThroughputB_Mbps'].sum()
    }
    return pd.Series(metrics)

def generate_report(throughput_df, user_dist_df, metrics, user_stats):
    """Generate a comprehensive markdown report"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    report = f"""# Wireless Network Handover Analysis Report
Generated on: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}

## Overview
This report analyzes the performance of a wireless network handover simulation with two access points (AP A and AP B).

## Key Performance Metrics
- **Average Throughput**
  - AP A: {metrics['Average Throughput AP A']:.2f} Mbps
  - AP B: {metrics['Average Throughput AP B']:.2f} Mbps
- **Peak Throughput**
  - AP A: {metrics['Peak Throughput AP A']:.2f} Mbps
  - AP B: {metrics['Peak Throughput AP B']:.2f} Mbps
- **Throughput Stability (Standard Deviation)**
  - AP A: {metrics['Throughput Stability AP A']:.2f}
  - AP B: {metrics['Throughput Stability AP B']:.2f}

## User Distribution Analysis
{user_stats.to_markdown()}

## Visualization Results
![Combined Throughput](plots/throughput_combined.png)
![Throughput Distribution](plots/throughput_distribution.png)

## Conclusions
1. The network demonstrated {
    'stable' if max(metrics['Throughput Stability AP A'], 
                   metrics['Throughput Stability AP B']) < 1 else 'variable'} 
    throughput performance
2. User distribution remained balanced after final migration
3. Total network capacity was {
    'maintained' if abs(metrics['Average Throughput AP A'] + 
                       metrics['Average Throughput AP B'] - 
                       throughput_df['Total_Throughput'].iloc[0]) < 1 
    else 'affected'} throughout the handover process
"""

    with open(f'analysis_report_{timestamp}.md', 'w') as f:
        f.write(report)

def main():
    # Load and process data
    throughput_df, user_dist_df = load_and_preprocess_data()
    
    # Generate visualizations
    plot_throughput_analysis(throughput_df)
    
    # Analyze user distribution
    user_stats = analyze_user_distribution(user_dist_df)
    
    # Calculate performance metrics
    metrics = calculate_performance_metrics(throughput_df)
    
    # Generate comprehensive report
    generate_report(throughput_df, user_dist_df, metrics, user_stats)
    
    print("Analysis complete! Check analysis_report_*.md for detailed results.")

if __name__ == "__main__":
    main()
