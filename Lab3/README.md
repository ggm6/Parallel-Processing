#Game of Life

Compile:
mpic++ Lab3.cpp -o Lab3

Example Run:
mpiexec -n 4 ./Lab3 pulsar.txt 5
  - "-n 4" = number of threads
  - "pulsar.txt" = example starting point for game of life
  - "5" = number of total generations
