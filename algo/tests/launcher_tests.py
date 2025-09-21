import subprocess

# Lista di comandi da inviare al programma worker
comandi = [
    "mpirun -np 3 --map-by ppr:1:node --hostfile ./mpi_hostfile ./merge_sort_distributed_test 5 1000000 merge_sort_distributed_test_one_core_O2 1000",
    "mpirun -np 12 --hostfile ./mpi_hostfile ./merge_sort_distributed_test 1000000 merge_sort_distributed_test_all_cores_O2 1000",
    "./merge_sort_serial_test 5 1000000 merge_sort_serial_test_O2 1000",
    "mpirun -np 3 --map-by ppr:1:node --hostfile ./mpi_hostfile ./matrix_multiplication_distributed_test 5 1000 matrix_multiplication_distributed_test_one_core_O2 5",
    "mpirun -np 12 --hostfile ./mpi_hostfile ./matrix_multiplication_distributed_test 5 1000 matrix_multiplication_distributed_test_all_cores_O2 5",
    "./matrix_multiplication_serial_test 5 1000 matrix_multiplication_serial_test_O2 5"
]

for cmd in comandi:
    print(f"[launcher] Lancio il comando: {cmd}")
    result = subprocess.run(
        ["python3", "automatic_tests.py"] + cmd.split()
        )
    if result.stderr:
        print("[launcher] Errori:", result.stderr.strip())
    print("-" * 40)
