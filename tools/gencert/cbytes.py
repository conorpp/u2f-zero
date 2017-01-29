#!/usr/bin/env python
from __future__ import print_function
"""
    cbytes.py

    Output a c file with the DER certificate.
    Read der file as input
"""
import sys,fileinput,binascii

if len(sys.argv) not in [2,3]:
    print('usage: %s <certificate.der|hex-input> [-s]' % sys.argv[0])
    print('    -s: just output c string (for general use)')
    sys.exit(1)

buf = None
try:
    buf = bytearray(open(sys.argv[1], 'rb').read())
except:
    n = sys.argv[1].replace('\n','')
    n = sys.argv[1].replace('\r','')
    buf = bytearray(binascii.unhexlify(n))

c_str = ''
size = len(buf)

a = ''.join(map(lambda c:'\\x%02x'%c, buf))

for i in range(0,len(a), 80):
    c_str += ("\""+a[i:i+80]+"\"\n")

if '-s' in sys.argv:
    print(c_str)
    sys.exit(0)

print('// generated')
print('#include <stdint.h>')
print()
print('code uint8_t __attest[] = \n%s;' % c_str)
print('const uint16_t __attest_size = sizeof(__attest)-1;')


