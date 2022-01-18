#!/bin/bash

./configure --without-readline --disable-assembly --disable-shared --prefix=/opt/gmp-6.2.1 --host=i386

patch < gmp-6.2.1.patch

