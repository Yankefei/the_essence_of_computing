#!/bin/bash


script_dir=$(cd $(dirname $0) && pwd)

script_file=$(echo $0 | awk -F / '{print $NF}')

cd ${script_dir}/build

./clean.sh && cmake .. && make  tools   -j2


if [ $? -ne 0 ]; then
    echo "make tools lib filed"
    exit -1
fi


mkdir ../tools_lib

cp tools/libtools.a  ../tools_lib

if [ $? -ne 0 ]; then
    echo "move tools lib filed"
    exit -1
fi

cd ..
