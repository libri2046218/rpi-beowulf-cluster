import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import sys
import os

if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} <results.csv>")
    sys.exit(1)

csv_file = sys.argv[1]

if not os.path.exists(csv_file):
    print(f"Error: file '{csv_file}' not found.")
    sys.exit(1)

# Carica dati
df = pd.read_csv(csv_file)
x = df["input"].values
y = df["time"].values

# Definizione modelli da testare
models = {
    "lineare": (lambda x, a, b: a*x + b, [1, 1]),
    "quadratico": (lambda x, a, b, c: a*x**2 + b*x + c, [1e-6, 1e-3, 1]),
    "logaritmico": (lambda x, a, b: a*np.log(x) + b, [1, 1]),
    "nlogn": (lambda x, a, b: a * x * np.log(x) + b, [1e-6, 1]),
    "esponenziale": (lambda x, a, b: a*np.exp(b*x), [1e-6, 1e-3]),
    "potenza": (lambda x, a, b: a * np.power(x, b), [1e-6, 2]),
}

results = {}

# Calcolo fit e R² per ogni modello
for name, (func, p0) in models.items():
    try:
        popt, _ = curve_fit(func, x, y, p0=p0, maxfev=10000)
        y_fit = func(x, *popt)
        residuals = y - y_fit
        ss_res = np.sum(residuals**2)
        ss_tot = np.sum((y - np.mean(y))**2)
        r2 = 1 - (ss_res / ss_tot)
        results[name] = (r2, popt, y_fit)
    except Exception as e:
        results[name] = (None, None, None)

# Trova modello migliore
best_model = max(results.items(), key=lambda kv: kv[1][0] if kv[1][0] is not None else -np.inf)

print("Risultati del fit:")
for name, (r2, popt, _) in results.items():
    if r2 is not None:
        print(f" - {name:12s}: R² = {r2:.4f}, parametri = {popt}")
    else:
        print(f" - {name:12s}: fallito")

print(f"\nMiglior modello: {best_model[0]} (R² = {best_model[1][0]:.4f})")

# Plot dati e tutti i fit validi
plt.figure(figsize=(10,6))
plt.scatter(x, y, label="Dati", color="black")

for name, (r2, popt, y_fit) in results.items():
    if r2 is not None:
        plt.plot(x, y_fit, label=f"{name} (R²={r2:.3f})")

plt.xlabel("Input size")
plt.ylabel("Time (seconds)")
plt.title(f"Fit dei dati - file {csv_file}")
plt.grid(True)
plt.legend()
plt.tight_layout()

out_file = os.path.splitext(csv_file)[0] + "_fit.png"
plt.savefig(out_file, dpi=150)
plt.show()

print(f"Grafico salvato in {out_file}")
