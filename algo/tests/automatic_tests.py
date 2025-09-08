import subprocess
import sys

if len(sys.argv) < 4:
    print(f"Usage: {sys.argv[0]} <mpi_command> [args...] <runs> <max_input>")
    print(f"Example: {sys.argv[0]} mpirun -np 4 ./program 5 1000")
    sys.exit(1)

# Parse CLI
base_cmd = sys.argv[1:-2]   # everything except last 2 args
runs = int(sys.argv[-2])    # number of runs per input
max_input = int(sys.argv[-1])  # maximum input value

with open("results.csv", "w") as f:
    f.write("input,time\n")  # CSV header

    for n in range(1, max_input + 1, 10):  # from 50 to max_input step 50
        for r in range(runs):
            # Build full command
            cmd = base_cmd + [str(n)]

            print(f"Dimensione input: {n}, Tentativo: {r}")

            # Run the command and capture output
            output = subprocess.check_output(cmd, text=True)

            # Look for "Runtime"
            for line in output.splitlines():
                if "time" in line:
                    time_val = line.split()[-1]
                    f.write(f"{n},{time_val}\n")
                    break
