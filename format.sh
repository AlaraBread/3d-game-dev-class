#!/bin/sh

cd src
clang-format -i `find . -regex ".*\.\(c\|h\)$" -type f`
