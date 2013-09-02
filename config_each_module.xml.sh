#! /bin/bash


echo '<application name="each_module" description="" cyclelimit="-1" gui="">'
ls modules/*/*.cpp| sort | sed "s/modules\/.*\/\(.*\)\.cpp/\1/g" | awk {'print "<module name=\""$1"\"><parameters><param name=\"class\">"$1"</param></parameters></module>" '}
echo '</application>'

