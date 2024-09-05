#!/bin/sh

cd "$(dirname "$0")"
root=`pwd`

cd ${root}
cd gfc/simple_json/src
make static

cd ${root}
cd gfc/simple_logger/src
make static

cd ${root}
cd gfc/src
make static
