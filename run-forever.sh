#!/bin/sh

if [ $# -eq 0 ]
then
	echo "missing parameter"
	exit 1
fi

while true
do
	./replayer $1
done
