#! /bin/bash

echo "// WARNING: This file is generated automatically !!"

echo "#ifndef ALL_MODULES_H"
echo "#define ALL_MODULES_H"


find modules*/  -maxdepth 2 -name "*.h" | sort | awk {'print "#include \""$1"\""'}

echo "Module * createNewModule(const ConfigReader& rx_configReader)"
echo {
echo "Module * tmp = NULL;"

echo "const string moduleClass = rx_configReader.SubConfig(\"parameters\").SubConfig(\"param\", \"class\").GetValue();"
echo "if(false){}"
find modules*/  -maxdepth 2 -name "*.cpp" -exec basename {} \; | sort | sed s/\.cpp//g | awk {'print "else if(moduleClass.compare(\""$1"\") == 0) {tmp = new "$1"(rx_configReader);} "'}

echo "else throw(\"Module type unknown : \" + moduleClass);"
echo "return tmp;"
echo }

echo "#endif"
