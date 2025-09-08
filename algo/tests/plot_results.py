import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} <results.csv>")
    sys.exit(1)

csv_file = sys.argv[1]

if not os.path.exists(csv_file):
    print(f"Error: file '{csv_file}' not found.")
    sys.exit(1)

# Carica il CSV
df = pd.read_csv(csv_file)

# Raggruppa per input e prende il minimo
min_times = df.groupby("input")["time"].min()

# Plot
plt.figure(figsize=(8,5))
plt.plot(min_times.index, min_times.values, "o-", label="Runtime (min)")

plt.style.use("seaborn-v0_8")

plt.xlabel("Input size")
plt.ylabel("Time (seconds)")
plt.title(f"Program Runtime vs Input Size (min over runs)\n{csv_file}")
plt.grid(True, which="both", linestyle="--", linewidth=0.6, alpha=0.7)
plt.legend()

# Nome file immagine basato sul CSV
output_file = os.path.splitext(csv_file)[0] + "_plot.png"
plt.savefig(output_file, dpi=150)
plt.show()

print(f"Plot salvato in {output_file}")
