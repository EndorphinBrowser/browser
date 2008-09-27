#!/bin/sh
#
# Hack of a script to find some common style errors
#
# Coding style: http://code.google.com/p/arora/wiki/CodingStyle
#

grep 'if(' * | grep -v Makefile | grep -v Binary
grep 'for(' * | grep -v Makefile | grep -v Binary
grep 'while(' * | grep -v Makefile | grep -v Binary
grep ' $' * | grep -v Makefile | grep -v Binary
grep '^{ }*{' * | grep -v Makefile | grep -v Binary
grep '){' * | grep -v Makefile | grep -v Binary
grep '	' * | grep -v Makefile | grep -v Binary
egrep '\(.*\* .*\)' * | grep -v Makefile | grep -v Binary | grep '::'
