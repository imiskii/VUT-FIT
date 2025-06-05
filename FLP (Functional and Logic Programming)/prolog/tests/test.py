# FLP - Rubik's cube solver
# Michal Ľaš (xlasmi00)
# Date: 11.04.2025
# File: test.py

import subprocess
import glob
import time
import os
import sys


TIMEOUT = 10000  # seconds


def is_valid_cube(flat_lines:str):
    tiles = []

    for line in flat_lines:
        parts = line.strip().split()
        if len(parts) == 0:
            continue
        tiles.extend(parts)

    if len(tiles) != 18:
        return False

    faces = {
        "top":    tiles[0:3],
        "front":  [tiles[3],  tiles[7],  tiles[11]],
        "right":  [tiles[4],  tiles[8],  tiles[12]],
        "back":   [tiles[5],  tiles[9],  tiles[13]],
        "left":   [tiles[6],  tiles[10], tiles[14]],
        "bottom": tiles[15:18]
    }


    for name, face in faces.items():
        if len(set(face)) != 1:
            return False

    return True


# Check test arguments
if len(sys.argv) < 3:
    print("Usage: python3 test_script.py <path_to_binary> <path_to_test_inputs>")
    sys.exit(1)

# Path to binary executable
EXECUTABLE = sys.argv[1]
INPUT_FOLDER = sys.argv[2]

# List of files with test cases
input_files = glob.glob(os.path.join(INPUT_FOLDER, "*.in"))

if not input_files:
    print(f"No input file found in directory {INPUT_FOLDER}")
    sys.exit(1)

input_files.sort()

# Test all input cases
for input_file in input_files:
    try:
        with open(input_file, "r") as f:
            input_data = f.read()

        start_time = time.time()

        result = subprocess.run(
            [EXECUTABLE],
            input=input_data.encode("utf-8"),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=TIMEOUT
        )

        end_time = time.time()
        elapsed_time = end_time - start_time

        output = result.stdout.decode("utf-8").strip()
        lines = output.splitlines()[-9:]  # last 9 lines, wich should be final cube

        if is_valid_cube(lines):
            print(f"[PASS]: {os.path.basename(input_file)}, runtime: {elapsed_time:.5f}s")
        else:
            print(f"[FAIL]: {os.path.basename(input_file)}, runtime: {elapsed_time:.5f}s")

    except subprocess.TimeoutExpired:
        print(f"[FAIL]: {os.path.basename(input_file)}, runtime: max time exceeded")
    except Exception as e:
        print(f"[FAIL]: {os.path.basename(input_file)}, error: {e}")
