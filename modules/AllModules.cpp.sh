#! /bin/bash


# This file

echo "// WARNING: This file is generated automatically !!"

echo
echo "#include \"Factories.h\""
echo

find $1/modules*/ -mindepth 1 -maxdepth 1 -type d | grep -v CMakeFiles | sort | xargs -i{} basename {} | awk {'print "#include \""$1"/"$1".h\""'}

echo
echo "void registerAllModules(FactoryModules& x_fact1, FactoryParameters& x_fact2)"
echo {

find $1/modules*/ -mindepth 1 -maxdepth 1 -type d | grep -v CMakeFiles | sort | xargs -i{} basename {} | awk {'print "\tx_fact1.Register<"$1">(\""$1"\");\n\tx_fact2.Register<"$1"::Parameters>(\""$1"\");"'}

echo }

