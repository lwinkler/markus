#! /bin/bash


# This file

echo "// WARNING: This file is generated automatically !!"

echo "#ifndef ALL_MODULES_H"
echo "#define ALL_MODULES_H"


find modules*/ -mindepth 1 -maxdepth 1 -type d | grep -v CMakeFiles | sort | xargs -i{} basename {} | awk {'print "#include \""$1"/"$1".h\""'}

echo
echo "void registerAllModules(FactoryModules& x_fact)"
echo {

find modules*/ -mindepth 1 -maxdepth 1 -type d | grep -v CMakeFiles | sort | xargs -i{} basename {} | awk {'print "\tx_fact.Register<"$1">(\""$1"\"); "'}

echo }

echo "#endif"
