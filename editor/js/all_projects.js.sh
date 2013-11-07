#! /bin/bash
# Generate the all_projects file automatically from the list of modules


echo "var availableProjectsNames = ["
find projects*/  -name "*.xml" -type f -exec ls -l {} \; 2> /dev/null | sort | awk {'print "\""$1"\","'}
echo "];"
