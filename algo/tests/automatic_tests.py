import subprocess
import sys
from datetime import datetime

if len(sys.argv) < 6:
    print(f"Usage: {sys.argv[0]} <mpi_command> [args...] <runs> <max_input> <test_name> <step/fixed_step>")
    print(f"Example (variable step): {sys.argv[0]} mpirun -np 4 ./program 5 1000 mytest variable")
    print(f"Example (fixed step): {sys.argv[0]} mpirun -np 4 ./program 5 1000 mytest 50")
    sys.exit(1)

# Parse CLI
# Ultimi 4 argomenti sono sempre: runs, max_input, test_name, step_arg
runs = int(sys.argv[-4])
max_input = int(sys.argv[-3])
test_name = sys.argv[-2]
step_arg = sys.argv[-1]

# Tutto il resto è il comando MPI + eventuali argomenti
base_cmd = sys.argv[1:-4]

# Determina se usare passo variabile o fisso
use_variable_step = step_arg.lower() == "variable"
fixed_step = None
if not use_variable_step:
    try:
        fixed_step = int(step_arg)
    except ValueError:
        print("Invalid fixed step value. Must be 'variable' or an integer.")
        sys.exit(1)

# Crea il nome del file con timestamp
timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
filename = f"results-{test_name}-{timestamp}.csv"

def variable_step(min_value, max_value):
    n = min_value
    while n <= max_value:
        yield n
        n += 10 ** (len(str(n)) - 1)

with open(filename, "w") as f:
    f.write("input,time\n")  # CSV header

    if use_variable_step:
        input_iterable = variable_step(1, max_input)
    else:
        input_iterable = range(1, max_input + 1, fixed_step)

    for n in input_iterable:
        for r in range(runs):
            cmd = base_cmd + [str(n)]
            print(f"Dimensione input: {n}, Tentativo: {r+1}")
            output = subprocess.check_output(cmd, text=True)
            for line in output.splitlines():
                if "time" in line:
                    time_val = line.split()[-1]
                    f.write(f"{n},{time_val}\n")
                    break

print(f"Risultati salvati in: {filename}")
