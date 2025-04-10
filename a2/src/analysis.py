import matplotlib.pyplot as plt
import pandas as pd

# 讀取 CSV
df_tp = pd.read_csv("throughput.csv")
df_ue = pd.read_csv("ue_count.csv")

# 計算平均 throughput
mean_tp_ap0 = df_tp["AP0_Throughput(Mbps)"].mean()
mean_tp_ap1 = df_tp["AP1_Throughput(Mbps)"].mean()

# 每個時間點的平均 throughput
df_tp["Avg_Throughput"] = (df_tp["AP0_Throughput(Mbps)"] + df_tp["AP1_Throughput(Mbps)"]) / 2

# === Throughput 折線圖 + 平均線 ===
plt.figure(figsize=(10, 5))
plt.plot(df_tp["Time(s)"], df_tp["AP0_Throughput(Mbps)"], label="AP32 Throughput")
plt.plot(df_tp["Time(s)"], df_tp["AP1_Throughput(Mbps)"], label="AP33 Throughput")
plt.plot(df_tp["Time(s)"], df_tp["Avg_Throughput"], label="Average Throughput", color="green", linestyle='--')
plt.xlabel("Time (s)")
plt.ylabel("Throughput (Mbps)")
plt.title("WiFi Throughput Over Time")
plt.legend()
plt.grid(True)
plt.savefig("throughput_plot.png")
plt.show()

# === 平均 throughput bar 圖 + 標註數值 ===
plt.figure(figsize=(6, 5))
aps = ["AP32", "AP33"]
avg_values = [mean_tp_ap0, mean_tp_ap1]
colors = ["blue", "orange"]
bars = plt.bar(aps, avg_values, color=colors)

# 在每個柱子上方加上數值標籤
for i, bar in enumerate(bars):
    height = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2, height + 0.5, f"{height:.2f}", 
             ha='center', va='bottom', fontsize=10)

plt.ylabel("Average Throughput (Mbps)")
plt.title("Average Throughput per AP")
plt.grid(axis="y")
plt.savefig("avg_throughput_bar.png")
plt.show()

# === UE 數量 點圖 ===
plt.figure(figsize=(10, 5))
plt.scatter(df_ue["Time(s)"], df_ue["AP0_UEs"], label="AP32 UEs", color='blue', s=10)
plt.scatter(df_ue["Time(s)"], df_ue["AP1_UEs"], label="AP33 UEs", color='orange', s=10)
plt.xlabel("Time (s)")
plt.ylabel("Number of Connected UEs")
plt.title("Number of UEs per AP Over Time")
plt.legend()
plt.grid(True)
plt.savefig("ue_count_scatter.png")
plt.show()
