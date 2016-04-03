#!/usr/bin/env python
from __future__ import print_function
"""
    cbytes.py

    converts binary input on STDIN
    to an equivilent C string
"""
import fileinput

buf = bytearray()

for l in fileinput.input():
    buf = buf + bytearray(l)

a = ''.join(map(lambda c:'\\x%02x'%c, buf))

for i in range(0,len(a), 80):
    print("\""+a[i:i+80]+"\"")
