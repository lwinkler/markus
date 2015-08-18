#! /bin/bash
# Generate the all_modules.js file automatically from the list of modules


echo "var availableModuleNames = ["
find modules*/ -mindepth 1 -maxdepth 1 -type d | grep -v CMakeFiles | sort | xargs -i{} basename {} | awk {'print "\""$1"\","'}
echo "];"
