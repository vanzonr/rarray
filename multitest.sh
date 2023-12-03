#!/bin/bash

compilers=(gcc/8 gcc/9 gcc/11 gcc/12 gcc/13 clang/17 intel/2019u1 intel/2019u2 intel/2019u3 intel/2019u4 intel/2019u5 intel/2020u1 intel/2020u2 intel/2020u3 intel/2020u4 intel/2020u5 intel/2021u1 intel/2021u2 intel/2021u3 intel/2021u4 intel/2021u5 intel/2022u1 intel/2022u2 intel/2022u3 intel/2022u4 intel/2022u5 intel/2023u1 intel/2023u2 intel/2023u3 intel/2023u4 intel/2023u5 oneapi/2022.3.0 oneapi/2023.1.0 oneapi/.experimental-2023.1.0 "cuda xl/16.1.1.10" "cuda gcc/11" "cuda/12.0.0 oneapi/.experimental-2023.1.0")

mydir=$PWD
mkdir -p alltestresults
for onemod in "${compilers[@]}"
do
    module purge >& /dev/null
    module load $onemod 2>/dev/null || module load ${onemod/\//\/.experimental-} 2>/dev/null || continue
    module load valgrind || true
    echo '========================'$onemod'====================='
    mkdir -p "alltestresults/$onemod"
    rm -rf "alltestresults/$onemod"
    mkdir -p "alltestresults/$onemod"
    pushd "alltestresults/$onemod"  >& /dev/null
    module list  >& "config.out"
    $mydir/configure 2>&1 >> "config.out"
    (time make test)   >& "test.output"
    (time make test23) >& "test23.output"
    (time make benchmarks) >& "benchmarks.output"
    popd  >& /dev/null
done
