#! /bin/bash

echo "// WARNING: This file is generated automatically !!"

echo "#ifndef ALL_MODULES_H"
echo "#define ALL_MODULES_H"


find modules*/ -mindepth 2  -maxdepth 2 -name "*.h" | sort | awk {'print "#include \""$1"\""'}

echo
echo "void registerAllModules(FactoryModules& x_fact)"
echo {

find modules*/ -mindepth 2  -maxdepth 2 -name "*.cpp"  -exec basename {} \; | sort | sed s/\.cpp//g | awk {'print "\tx_fact.RegisterModule<"$1">(\""$1"\"); "'}

echo }

echo "#endif"
