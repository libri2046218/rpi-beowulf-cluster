import subprocess
import sys
from datetime import datetime

if len(sys.argv) < 6:
    print(f"Usage: {sys.argv[0]} <command> [args...] <runs> <max_input> <test_name> <step>")
    print(f"Example (fixed step): {sys.argv[0]} mpirun -np 4 ./program 5 1000 mytest 50")
    sys.exit(1)

# Parse CLI
# Ultimi 4 argomenti sono sempre: runs, max_input, test_name, step_arg
runs = int(sys.argv[-4])
max_input = int(sys.argv[-3])
test_name = sys.argv[-2]
fixed_step = int(sys.argv[-1])

# Tutto il resto è il comando per eseguire + eventuali argomenti
base_cmd = sys.argv[1:-4]

# Crea il nome del file con timestamp
timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
filename = f"results-{test_name}-{timestamp}.csv"
log = f"log-{test_name}-{timestamp}.txt"


with open(filename, "w") as f, open(log, "w") as f_log:
    f.write("input,time\n")  # CSV header

    input_iterable = range(1, max_input + 1, fixed_step)

    for n in input_iterable:
        for r in range(runs):
            cmd = base_cmd + [str(n)]
            print(f"Dimensione input: {n}, Tentativo: {r+1}")
            output = subprocess.check_output(cmd, text=True)
            for line in output.splitlines():
                f_log.write(line+"\n")
                if "time" in line:
                    time_val = line.split()[-1]
                    f.write(f"{n},{time_val}\n")
                    break
            f_log.write("-"*40 + "\n")

print(f"Risultati salvati in: {filename}")
