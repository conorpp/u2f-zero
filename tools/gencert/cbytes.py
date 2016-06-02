#!/usr/bin/env python
from __future__ import print_function
"""
    cbytes.py

    Output a c file with the DER certificate.
    Read der file as input
"""
import sys,fileinput

if len(sys.argv) != 2:
    print('usage: %s <certificate.der>' % sys.argv[0])
    sys.exit(1)

buf = bytearray(open(sys.argv[1], 'rb').read())

c_str = ''
size = len(buf)

a = ''.join(map(lambda c:'\\x%02x'%c, buf))

for i in range(0,len(a), 80):
    c_str += ("\""+a[i:i+80]+"\"\n")

print('// generated')
print('#include <stdint.h>')
print()
print('code uint8_t __attest[] = \n%s;' % c_str)
print('const uint16_t __attest_size = sizeof(__attest)-1;')
