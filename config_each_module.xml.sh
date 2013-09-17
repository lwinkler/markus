#! /bin/bash


echo '<application name="each_module" description="" cyclelimit="-1" gui="">'
find modules*  -maxdepth 2 -name "*.cpp" -exec basename {} \; | sort | sed s/\.cpp//g | awk {'print "<module name=\""$1"\"><parameters><param name=\"class\">"$1"</param></parameters></module>" '}
echo '</application>'

