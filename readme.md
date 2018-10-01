# Logic, Synthesis and Verification Programming Assignment

This is a repository to store the source code of NTU course LSV PA.

The PA is based on [abc](https://github.com/berkeley-abc/abc.git), and the source code related to the PA is in the **lsv** directory of **src**.

## PA Source Codes
- lsvCmd.cpp: abc command interface
- lsvMajFind.cpp: PA1, find the 3 input majority gate in a circuit
  - the code is correct
- lsv1SubFind.cpp: PA2, find the substitute candidates of all circuit node
  - the code is correct, but the run time is not well, so it fails the hidden testcase of PA
- satSolverExample.cpp: a simple example to demonstate how to use minisat API in abc
- final: source codes of final project
  - MUX decomposition using BDD
  - the project can just deal with toy example
  - the codes are not good for reference

last modified by flotisable
Mon Oct  1 21:06:25 CST 2018
