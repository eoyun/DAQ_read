# DAQ_read
structure
- src : read the DAQ file in my directory
- include : header file
- analysis/test : work directory

## How to set
in KNU server, set the environment and compile library code 

```bash
source env.sh 
make install
```

## How to use
in argument in makefile, type only code name without .cc

```bash
cd analysis 
make ARG=<code name> 
./<code name> <arg1> <arg2> ...
```

## for example
drs.cc -> local calibration

```bash
make ARG=drs 
./drs 12763 9 2 
#./drs <runnum> <mid> <ch>
```

