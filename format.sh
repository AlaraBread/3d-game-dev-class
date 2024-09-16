#!/bin/sh

cd "$(dirname "$0")"

cd src
clang-format -i `find . -regex ".*\.\(c\|h\)$" -type f`
