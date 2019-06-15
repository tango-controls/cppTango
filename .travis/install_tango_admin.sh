#!/usr/bin/env bash

echo "Install tango"
docker exec --user root cpp_tango make -C /home/tango/src/build install
if [ $? -ne 0 ]
then
  exit 1
fi

admin_dir="/home/tango/tango_admin"

echo "Build tango_admin"
docker exec cpp_tango mkdir -p "${admin_dir}/build"
docker exec cpp_tango cmake -H"${admin_dir}" -B"${admin_dir}/build"
docker exec cpp_tango make -C "${admin_dir}/build"
if [ $? -ne 0 ]
then
  exit 1
fi

echo "Install tango_admin"
docker exec --user root cpp_tango cp "${admin_dir}/build/tango_admin" /usr/local/bin
if [ $? -ne "0" ]
then
  exit 1
fi
