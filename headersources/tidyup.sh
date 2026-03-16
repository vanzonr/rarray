#!/bin/bash

set -uo pipefail

for i in *.h; do
    if ! [ -h "${i}pp" ]; then
        ln -sT "$i" "${i}pp"
    fi
done

for i in *.h; do
    for checkset in bugprone modernize readability performance cppcoreguidelines ; do
        clang-tidy --use-color=1 --checks="${checkset}-*" "${i}pp" -- -std=c++11 > "tidy-$checkset-$i.out" &
    done
    wait
done

find -name 'tidy-*.out' -empty -delete
