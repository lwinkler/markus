#! /bin/bash


# This file

echo "// WARNING: This file is generated automatically !!"

echo
echo "#include \"Factories.h\""
echo

xargs -a modules.txt -I{} basename {} | sort | awk {'print "#include \""$1"/"$1".h\""'}

echo
echo "void registerAllModules(FactoryModules& x_fact1, FactoryParameters& x_fact2)"
echo {

xargs -a modules.txt -I{} basename {} | sort | awk {'print "\tx_fact1.Register<mk::"$1">(\""$1"\");\n\tx_fact2.Register<mk::"$1"::Parameters>(\""$1"\");"'}

echo }

