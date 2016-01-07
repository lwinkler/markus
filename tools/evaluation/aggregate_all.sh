#! /bin/bash

# This script can be used to generate all summaries (in HTML format) of a simulation.
# It reads the .txt files that contain the list of files to aggregate.

echo "Aggregate all results in $1"
tools/evaluation/aggregate.py $1/results --no-browser 

echo "Aggregate all results in $1 by file"
find $1/ -maxdepth 1 -name "*.txt"|sed 's/.*\/\(.*\).txt/\1/'|xargs -i{} tools/evaluation/aggregate.py $1/results -f $1/{}.txt -o {}.html --no-browser
