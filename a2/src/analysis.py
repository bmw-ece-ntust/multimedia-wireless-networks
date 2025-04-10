import pandas as pd
import matplotlib.pyplot as plt
import re

# Analyze throughput log
def analyze_throughput_log(file_path):
    try:
        data = pd.read_csv(file_path)
        print("Throughput Analysis:")
        print(data.groupby("Node")["Throughput (Mbps)"].mean())
        data.pivot(index="Time", columns="Node", values="Throughput (Mbps)").plot()
        plt.title("Throughput Over Time")
        plt.xlabel("Time (s)")
        plt.ylabel("Throughput (Mbps)")
        plt.legend(title="Node")
        plt.grid()
        plt.show()
    except Exception as e:
        print(f"Error analyzing throughput log: {e}")

# Analyze user distribution log
def analyze_user_distribution_log(file_path):
    try:
        with open(file_path, "r") as file:
            content = file.read()
        time_blocks = content.split("================")
        user_distribution = {}
        for block in time_blocks:
            time_match = re.search(r"Time: (\d+)s", block)
            if time_match:
                time = int(time_match.group(1))
                node_a_users = len(re.findall(r"STA-A\d+", block))
                node_b_users = len(re.findall(r"STA-B\d+", block))
                user_distribution[time] = {"Node A": node_a_users, "Node B": node_b_users}
        df = pd.DataFrame(user_distribution).T
        print("User Distribution Over Time:")
        print(df)
        df.plot(kind="bar", stacked=True)
        plt.title("User Distribution Over Time")
        plt.xlabel("Time (s)")
        plt.ylabel("Number of Users")
        plt.legend(title="Node")
        plt.grid()
        plt.show()
    except Exception as e:
        print(f"Error analyzing user distribution log: {e}")

# Analyze user movement log
def analyze_user_movement_log(file_path):
    try:
        with open(file_path, "r") as file:
            content = file.readlines()
        movements = [line.strip() for line in content if "moved from" in line]
        print("User Movement Details:")
        for movement in movements:
            print(movement)
    except Exception as e:
        print(f"Error analyzing user movement log: {e}")

# Main function to call analysis functions
def main():
    throughput_log = "throughput_log.txt"
    user_distribution_log = "user_distribution_log.txt"
    user_movement_log = "user_movement_log.txt"

    print("Analyzing Throughput Log...")
    analyze_throughput_log(throughput_log)

    print("\nAnalyzing User Distribution Log...")
    analyze_user_distribution_log(user_distribution_log)

    print("\nAnalyzing User Movement Log...")
    analyze_user_movement_log(user_movement_log)

if __name__ == "__main__":
    main()
