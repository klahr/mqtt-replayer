#!/bin/sh

if [ $# -eq 0 ]
then
	echo "missing parameter"
	exit 1
fi

for (( ; ; ))
do
	./replayer $1
done
