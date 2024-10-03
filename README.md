# Secure Settlement Registry Prototype
This repository implements a basic prototype secure settlement registry,
using EMP-Toolkit.

## Quick Review

The logic of the program is simple and can be reviewed quickly
in test/ssrep.cpp, specifically the process_submissions function.

## Generating Sample Data

A sample data of settlement can be generated:

`python gen.py [settlement_count]`

For example, to generate a sample dataset in the current directory with 2000 settlements:

`python gen.py 2000`

Or, to organize the data:

`mkdir data_2k; cd data_2k; python gen.py 2000`

## Running MPC computation
To perform a run of the MPC computation, run two processes:

Process A:
`bin/ssrep 1 [settlement_data_dir] [PORT_TO_LISTEN]`

Process B:
`bin/ssrep 2 [settlement_data_dir] [PORT_ON_A] [IP_TO_A]`

The 'settlement_data_dir' should refer to the exact same generated settlement data set, 
except that A only needs access to the 'submission_bits_a.txt' file,
and B only needs access to the 'submission_bits_b.txt' file.

For example, to run on localhost and using the data from 'data_2k', run as follow:

Process A:
`bin/ssrep 1 data_2k/ 9999`

Process B:
`bin/ssrep 2 data_2k/ 9999 127.0.0.1`

The programs will connect and execute an MPC computation.
Results will be printed to the console.

## Benchmark run

To run a full benchmark run, first compile the ssrep program, then
use the included mem.sh to monitor usage. For example, to
run on localhost using the 'data_2k' dataset:

Process A:
`make ssrep && mem.sh bin/ssrep 1 data_2k/ 9999`

Process B:
`make ssrep && mem.sh bin/ssrep 2 data_2k/ 9999 127.0.0.1`
