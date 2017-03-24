#!/usr/bin/env bash

stress -c 1 --timeout 8 & cpulimit -p $( pidof -o $! stress ) -l $1
