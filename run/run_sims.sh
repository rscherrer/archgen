#!/usr/bin/env bash

# This script runs multiple simulations using the specified
# executable and parameter files. It renames the output files
# to include the simulation index for better organization.
# The first argument is the path to the executable (default: ./archgen).
# The second argument is the directory containing parameter files (default: current directory).
# Parameter files must be named in the format: sim_<number>_parameters.txt.
# If an architecture must be supplied to all simulations, it should be
# present in the working directory as "architecture.txt". If different
# architectures should be supplied for different simulations, they
# should be present as files named "sim_<number>_architecture.txt" in the working directory.
# The script will keep simulation identifiers as prefixes for all output files.

set -euo pipefail

EXECUTABLE="${1:-./archgen}"
PARAM_DIR="${2:-.}"

if [[ ! -x "$EXECUTABLE" ]]; then
	echo "Error: executable not found or not executable: $EXECUTABLE" >&2
	echo "Usage: $0 [executable_path] [parameters_directory]" >&2
	exit 1
fi

mapfile -t PARAM_FILES < <(find "$PARAM_DIR" -maxdepth 1 -type f -name 'sim_*_parameters.txt' | sort -V)

if [[ ${#PARAM_FILES[@]} -eq 0 ]]; then
	echo "Error: no files matching sim_*_parameters.txt found in: $PARAM_DIR" >&2
	exit 1
fi

echo "Running ${#PARAM_FILES[@]} simulation(s) with: $EXECUTABLE"

OUTPUT_PREFIXES=(traits genotypes alleles architecture paramlog)
WORK_DIR="$(pwd)"

for param_file in "${PARAM_FILES[@]}"; do
	echo "----------------------------------------"
	echo "Running with: $param_file"
	param_basename="$(basename "$param_file")"
	if [[ "$param_basename" =~ ^sim_([0-9]+)_parameters\.txt$ ]]; then
		sim_index="${BASH_REMATCH[1]}"
	else
		echo "Error: parameter file name must match sim_<number>_parameters.txt: $param_basename" >&2
		exit 1
	fi

	architecture_restored=false
	existing_arch_file="$WORK_DIR/sim_${sim_index}_architecture.txt"
	if [[ -f "$existing_arch_file" ]]; then
		cp "$existing_arch_file" "$WORK_DIR/architecture.txt"
		architecture_restored=true
		echo "Prepared architecture.txt from $(basename "$existing_arch_file")"
	fi

	"$EXECUTABLE" "$param_file"

	shopt -s nullglob
	for prefix in "${OUTPUT_PREFIXES[@]}"; do
		for output_file in "$WORK_DIR"/"$prefix"*; do
			output_basename="$(basename "$output_file")"
			if [[ "$output_basename" == "architecture.txt" && "$architecture_restored" == false ]]; then
				echo "Leaving architecture.txt untouched (no sim-specific architecture restored for sim_${sim_index})."
				continue
			fi
			renamed_file="$WORK_DIR/sim_${sim_index}_${output_basename}"
			mv -f "$output_file" "$renamed_file"
			echo "Renamed: $output_basename -> $(basename "$renamed_file")"
		done
	done
	shopt -u nullglob
done

echo "----------------------------------------"
echo "All simulations completed successfully."
