#! /bin/bash
# Generate the all_modules.js file automatically from the list of modules


echo "var availableModulesNames = ["
find modules*/  -maxdepth 2 -name "*.cpp" -exec basename {} \; | sort | sed s/\.cpp//g | sed "s/modules\/.*\/\(.*\)\.cpp/\1/g" | awk {'print "\""$1"\","'}
echo "];"
