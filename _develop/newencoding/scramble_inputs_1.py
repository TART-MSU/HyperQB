import subprocess
import re
import os
import random
import csv
SEED = 42
random.seed(SEED)


def replace_forall_exists_line(line, scrambled_inputs):
    f_or_e = ""
    if "forall" in line:
        f_or_e = "forall"
    if "exists" in line:
        f_or_e = "exists"

    inputs = ', '.join([str(x) for x in scrambled_inputs])
    return f_or_e + '(' + inputs + ')\n'


def run_command(file_path, timeout_seconds):
    command = f'time ../build/quabs {file_path}'

    try:
        process = subprocess.run(command, shell=True, timeout=timeout_seconds, text=True, capture_output=True)
        time_taken = process.stderr.strip('\nreal\t').split('\n')[0]
        stderr = process.stderr.strip().replace('\n', ', ').replace('\t', ': ')
        return file_path, time_taken, f'Success', stderr, SEED

    except subprocess.TimeoutExpired:
        return file_path, None, f'Timeout ({timeout_seconds} seconds)', None, SEED

def shuffle_orderings(input_file, output_directory):
    with open(input_file, 'r') as file:
        lines = file.readlines()

    perms = []
    for i, line in enumerate(lines):
        if "forall" in line or "exists" in line:
            numbers = [int(x) for x in line.split('(')[1].split(')')[0].split(', ')]
            perms.append(numbers)

    shuffles = []
    for _ in range(1000):
        random_orderings = []
        for inputs in perms:
            shuffled_list = inputs.copy()
            random.shuffle(shuffled_list)
            random_orderings.append(shuffled_list)
        shuffles.append(random_orderings)

    with open('results.csv', 'w', newline='') as csvfile:
        csv_writer = csv.writer(csvfile)
        csv_writer.writerow(['ID', 'Permutation', 'Elapsed Time (s)', 'Status', 'Command Output', 'SEED'])

        for idx, permutation in enumerate(shuffles):
            # output_file = os.path.join(output_directory, f"output_permutation_{idx + 1}.qcir")
            output_file = os.path.join(output_directory, f"scrambled.qcir")
            with open(output_file, 'w') as out_file:
                line_no = 0
                for i, line in enumerate(lines):
                    if "forall" in line or "exists" in line:
                        new_line = replace_forall_exists_line(line, permutation[line_no])
                        out_file.write(new_line)
                        line_no += 1
                        continue
                    out_file.write(line)

            # running quabs
            timeout = 1 # seconds
            result = (idx, ) + run_command(output_file, timeout)
            csv_writer.writerow(result)


if __name__ == "__main__":
    input_file = "bakery/bakery_YY.qcir"
    output_directory = "ex"

    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    shuffle_orderings(input_file, output_directory)

    import pandas as pd
    from tabulate import tabulate

    df = pd.read_csv("results.csv")

    table = tabulate(df, headers='keys', tablefmt='pretty', showindex=False)

    print(table)
