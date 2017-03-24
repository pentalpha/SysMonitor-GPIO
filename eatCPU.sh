#!/usr/bin/env bash
#Usage: eatCPU.sh %
#  where: % = maximum of cpu to be "eaten" {0 <= % <= 100}

stress -c 1 --timeout 8 & cpulimit -p $( pidof -o $! stress ) -l $1
