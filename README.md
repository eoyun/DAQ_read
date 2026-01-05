# DAQ_read
structure
- src : read the DAQ file in my directory
- include : header file
- analysis/test : work directory

## How to set
in KNU server, set the environment and compile library code 

```bash
source env.sh 
make
```

## How to use
in argument in makefile, type only code name without .cc

```bash
cd analysis 
make ARG=<code name> 
./<exe file> <arg1> <arg2> ...
```

## for example
drs.cc -> local calibration

```bash
make ARG=drs 
./drs_program 12763 9 2 
#./drs <runnum> <mid> <ch>
```

