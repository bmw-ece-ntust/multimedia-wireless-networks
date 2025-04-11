import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# 讀取 CSV 檔案
df = pd.read_csv('ue_positions.csv')  # 改成你的檔名或路徑

# 把 Time 排序，準備逐格動畫
time_steps = sorted(df['Time'].unique())

# 圖片大小與設定
fig, ax = plt.subplots(figsize=(12, 6))
ax.set_xlim(0, df['X'].max() + 50)
ax.set_ylim(df['Y'].min() - 20, df['Y'].max() + 20)
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_title("UE movement over time")
scat = ax.scatter([], [], c='blue')
texts = []

# 每個 UE 的歷史軌跡 (dict)
trajectories = {i: [] for i in df['UE_Index'].unique()}

def update(frame):
    ax.clear()
    t = time_steps[frame]
    subset = df[df['Time'] == t]

    ax.set_xlim(0, df['X'].max() + 50)
    ax.set_ylim(df['Y'].min() - 20, df['Y'].max() + 20)
    ax.set_title(f"UE movement at Time={t}")
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.grid(True)

    # 更新軌跡
    for _, row in subset.iterrows():
        trajectories[row['UE_Index']].append((row['X'], row['Y']))
        traj_x, traj_y = zip(*trajectories[row['UE_Index']])
        ax.plot(traj_x, traj_y, alpha=0.5)  # 畫歷史軌跡

    # 畫目前位置與 UE_Index
    ax.scatter(subset['X'], subset['Y'], c='blue')
    for _, row in subset.iterrows():
        ax.text(row['X'] + 2, row['Y'] + 2, str(row['UE_Index']), fontsize=7)

ani = animation.FuncAnimation(fig, update, frames=len(time_steps), interval=1000, repeat=False)

# 儲存成 GIF（需安裝 imagemagick 或 pillow）
ani.save('ue_movement.gif', writer='pillow', fps=1)

plt.close()
print("✅ 已完成 ue_movement.gif")
