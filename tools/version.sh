# /bin/bash

echo -n "git branch: ";
git rev-parse --abbrev-ref HEAD
echo -n "git commit: ";
git log --pretty=oneline -1 --abbrev-commit HEAD | cat
echo -n "git descr : ";
git describe --all --long HEAD
echo -n "Built on  : ";
uname -a
echo -n "Date      : ";
date

echo
echo "Last 5 commits"
echo "--------------"
git log --pretty=format:'%aD <%an> %s %d' -5 --abbrev-commit HEAD | cat
# git log -5 --color --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit HEAD -- | cat

echo
echo
echo "Difference since last commit"
echo "----------------------------"
git diff HEAD|cat
