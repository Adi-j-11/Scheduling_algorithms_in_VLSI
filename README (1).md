# Scheduling Algorithms for VLSI Design

## Overview
It is a tool for processing Directed Acyclic Graphs (DAGs) and performing scheduling heuristics such as ASAP (As-Soon-As-Possible) and ALAP (As-Late-As-Possible). It includes implementations in C and a Python script for visualization.

## Folder Structure
```
Scheduling_Algorithms_for_VLSI /
|-- ALAP.c              # C program implementing ALAP scheduling
|-- ASAP1.c             # C program implementing ASAP scheduling
|-- asap_input.txt      # Input data for ASAP scheduling
|-- dag.dot             # DOT file representing the DAG
|-- DAG_reader.c        # C program to read and parse DAGs
|-- display.py          # Python script for DAG visualization
|-- heuristics.c        # C program containing heuristic scheduling algorithms
|-- input.txt           # General input file
|-- intermediate.txt    # Intermediate data output
|-- output.txt          # Final output results
```

## Usage
### Compiling the C Programs
To compile the C programs, use a standard GCC compiler:
```sh
gcc -o ALAP ALAP.c
gcc -o ASAP1 ASAP1.c
```
### Running the Scheduling Algorithms
Run the compiled executables with the appropriate input files:
```sh
./ALAP < input.txt
./ASAP1 < asap_input.txt
```
### Visualizing the DAG
The `display.py` script can be used to visualize the DAG from the `dag.dot` file:
```sh
python3 display.py
```

## Dependencies
- GCC for compiling C programs
- Python 3 for visualization
- Graphviz (optional) for handling DOT files

## Author
Developed as part of a scheduling and DAG analysis project.

