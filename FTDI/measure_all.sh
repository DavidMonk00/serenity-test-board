#!/usr/bin/env bash


PATH=/home/dmonk/serenity-test-board/FTDI
export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH
$PATH/bin/main -l -N $1
