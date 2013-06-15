#!/bin/sh

rm demo.tests
gcc -o demo.tests simple_tests.c -Wall -pedantic  -lherc -rdynamic -I../include -L..

