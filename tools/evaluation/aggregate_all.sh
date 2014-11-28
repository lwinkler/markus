#! /bin/bash

echo "Aggregate all results in $1"
tools/evaluation/aggregate.py $1/results --no-browser 

echo "Aggregate all results in $1 by file"
find $1 -name "*.txt"|sed 's/.*\/\(.*\).txt/\1/'|xargs -i{} tools/evaluation/aggregate.py $1/results -f $1/{}.txt -o {}.html --no-browser
