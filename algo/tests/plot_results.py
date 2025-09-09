import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

if len(sys.argv) < 3:
    print(f"Usage: {sys.argv[0]} <min|avg> <baseline.csv> [other1.csv ...]")
    sys.exit(1)

agg_method = sys.argv[1].lower()
if agg_method not in ["min", "avg", "mean"]:
    print("Error: first argument must be 'min' or 'avg'")
    sys.exit(1)

csv_files = sys.argv[2:]
baseline_file = csv_files[0]
other_files = csv_files[1:]

plt.style.use("seaborn-v0_8")
colors = plt.cm.tab10.colors  # Up to 10 distinct colors

# -----------------------------
# First: Runtime plot
# -----------------------------
plt.figure(figsize=(10,6))
for idx, csv_file in enumerate(csv_files):
    if not os.path.exists(csv_file):
        print(f"Warning: file '{csv_file}' not found. Skipping.")
        continue

    df = pd.read_csv(csv_file)
    agg_times = df.groupby("input")["time"].min() if agg_method == "min" else df.groupby("input")["time"].mean()

    color = colors[idx % len(colors)]
    plt.plot(agg_times.index, agg_times.values, "o-", color=color, label=os.path.basename(csv_file))

plt.xlabel("Input size")
plt.ylabel("Time (seconds)")
plt.title(f"Program Runtime vs Input Size ({agg_method} over runs)")
plt.grid(True, which="both", linestyle="--", linewidth=0.6, alpha=0.7)
plt.legend()
output_runtime_file = f"combined_plot_{agg_method}.png"
plt.savefig(output_runtime_file, dpi=150)
plt.show()
print(f"Runtime plot salvato in {output_runtime_file}")

# -----------------------------
# Second: Speedup plot
# -----------------------------
if other_files:
    plt.figure(figsize=(10,6))

    # Load baseline
    baseline_df = pd.read_csv(baseline_file)
    baseline_times = baseline_df.groupby("input")["time"].min() if agg_method == "min" else baseline_df.groupby("input")["time"].mean()

    for idx, csv_file in enumerate(other_files):
        if not os.path.exists(csv_file):
            continue
        df = pd.read_csv(csv_file)
        agg_times = df.groupby("input")["time"].min() if agg_method == "min" else df.groupby("input")["time"].mean()

        # Align with baseline inputs
        common_inputs = baseline_times.index.intersection(agg_times.index)
        speedup = baseline_times[common_inputs] / agg_times[common_inputs]

        color = colors[idx % len(colors)]
        plt.plot(common_inputs, speedup.values, "o-", color=color, label=f"{os.path.basename(csv_file)} / {os.path.basename(baseline_file)}")

    plt.xlabel("Input size")
    plt.ylabel("Speedup")
    plt.title(f"Speedup vs Baseline ({os.path.basename(baseline_file)})")
    plt.grid(True, which="both", linestyle="--", linewidth=0.6, alpha=0.7)
    plt.axhline(1.0, color="black", linestyle="--", linewidth=0.8)  # reference line
    plt.legend()

    output_speedup_file = f"speedup_plot_{agg_method}.png"
    plt.savefig(output_speedup_file, dpi=150)
    plt.show()
    print(f"Speedup plot salvato in {output_speedup_file}")
