#!/bin/bash

set -e

# 如果没有 build 目录，则创建
if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

# 删除之前编译的文件
rm -rf `pwd`/build/*

cd `pwd`/build && 
    cmake .. &&
    make

# 回到项目根目录
cd ..

# 把头文件拷贝到 /usr/include/myMuduo
# .so 库拷贝到 /usr/lib 
if [ ! -d /usr/include/myMuduo ]; then
    mkdir /usr/include/myMuduo
fi

for header in `ls *.h`
do
    cp $header /usr/include/myMuduo
done

cp `pwd`/dir/libmymuduo.so /usr/lib

# 刷新动态库缓存
ldconfig