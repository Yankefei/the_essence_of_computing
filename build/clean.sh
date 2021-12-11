#!/bin/bash


script_dir=$(cd $(dirname $0) && pwd)


script_file=$(echo $0 | awk -F / '{print $NF}')


cd ${script_dir}

ls | grep -v ${script_file} | xargs rm -r  >> /dev/null 2>&1

exit 0
