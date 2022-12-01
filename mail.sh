#!/usr/bin/env bash

# fake mail script

if [ $# != 1 ]
then
	echo "usage: $0 mail@address" 1>&2
	exit 1
fi

cat > /dev/null
echo -n "sending to $1..."
sleep 1
echo " done"
exit 0

