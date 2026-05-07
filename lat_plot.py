import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the data
df = pd.read_csv('./lat_data/lat_orig.csv')

# Calculate statistics
p50 = df['latency'].median()
p95 = np.percentile(df['latency'], 95)
p99 = np.percentile(df['latency'], 99)
p999 = np.percentile(df['latency'], 99.9)

print(f"P50 (Median): {p50:.2f} cycles")
print(f"P99 (Tail):   {p99:.2f} cycles")
print(f"P99.9 (Extreme): {p999:.2f} cycles")

# Plot 1: Histogram (Log scale to see the tail)
plt.figure(figsize=(12, 6))
plt.subplot(1, 2, 1)
plt.hist(df['latency'], bins=100, color='skyblue', edgecolor='black', log=True)
plt.axvline(p99, color='red', linestyle='dashed', linewidth=1, label=f'P99: {p99:.0f}')
plt.title('Latency Distribution (Log Scale)')
plt.xlabel('Cycles')
plt.ylabel('Frequency')
plt.legend()

# Plot 2: CDF (Cumulative Distribution Function)
plt.subplot(1, 2, 2)
sorted_data = np.sort(df['latency'])
yvals = np.arange(len(sorted_data)) / float(len(sorted_data) - 1)
plt.plot(sorted_data, yvals, color='darkblue')
plt.title('Cumulative Distribution (CDF)')
plt.xlabel('Cycles')
plt.ylabel('Probability')
plt.grid(True, which="both", ls="-", alpha=0.5)

plt.tight_layout()
plt.savefig('./lat_data/lat_v0.png')
plt.show()