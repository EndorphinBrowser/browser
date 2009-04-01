#!/bin/sh
#
# Hack of a script to find some common style errors
#
# Coding style: http://code.google.com/p/arora/wiki/CodingStyle
#

options="-n --include=*.cpp --include=*.h -r"

grep $options 'if(' *
grep $options 'for(' *
grep $options 'while(' *
grep $options 'switch(' *
grep $options 'foreach(' *
grep $options ' $' *
grep $options '^{ }*{' *
grep $options '){' *
grep $options '	' *
egrep $options '\(.*\* .*\)' * | grep '::'

# var *name;
grep $options '[^\* \/]\* ' *
grep $options '[^& ]& ' *
