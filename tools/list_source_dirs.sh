#! /bin/bash
#
# This script lists all sources directories of Markus. It is used by commands:
#
# 	make lint
# 	make check

echo bulk
echo util
echo tests
echo main.cpp
echo editor
echo gui
cat modules.txt

