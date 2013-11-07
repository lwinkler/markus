#! /bin/bash
# Generate the all_projects file automatically from the list of modules


echo "var availableProjectsNames = ["
find projects*/  -name "*.xml" | sort | awk {'print "\""$1"\","'}
echo "];"
