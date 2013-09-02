#! /bin/bash
# Generate the all_modules.js file automatically from the list of modules


echo "var availableModulesNames = ["
ls modules/*/*.cpp| sort | sed "s/modules\/.*\/\(.*\)\.cpp/\1/g" | awk {'print "\""$1"\","'}
echo "];"
