#! /bin/bash
#
# This script lists all sources directories of Markus. It is used by commands:
#
# 	make lint
# 	make check
# 	make todo

echo bulk
echo util
echo tests
echo main.cpp
echo gui
cat modules.txt

