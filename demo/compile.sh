#!/bin/sh

rm demo.tests
gcc -o demo.tests *.c -Wall -pedantic  -lherc -rdynamic -I../include -L..

