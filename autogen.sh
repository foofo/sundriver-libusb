#!/bin/bash
autoscan
autoheader
aclocal
autoconf
libtoolize
automake --add-missing
./configure
