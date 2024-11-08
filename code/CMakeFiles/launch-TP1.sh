#!/bin/sh
bindir=$(pwd)
cd /mnt/c/Users/snsdt/Desktop/Projet_Moteur/code/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /mnt/c/Users/snsdt/Desktop/Projet_Moteur/code/main 
	else
		"/mnt/c/Users/snsdt/Desktop/Projet_Moteur/code/main"  
	fi
else
	"/mnt/c/Users/snsdt/Desktop/Projet_Moteur/code/main"  
fi
