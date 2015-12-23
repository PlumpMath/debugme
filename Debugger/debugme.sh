#!/bin/sh
cd `dirname $0`

echo "Debugme: Starting Debugger"
echo "My PID: $$, Parent PID: $PPID"
gdb -p $PPID -x gdb.script
