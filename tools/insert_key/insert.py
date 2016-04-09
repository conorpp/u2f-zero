#!/usr/bin/env python
from __future__ import print_function
import sys,os

if len(sys.argv) != 4:
    print('usage: %s <filename> <inserted file> <outputfile>' % sys.argv[0])
    sys.exit(1)

f = open(sys.argv[1], 'rw')
ins = open(sys.argv[2], 'r')

nf = ''

state = 1

for l in f.readlines():

    if '{{{' in l:
        nf += '// {{{\n'
        state = 2
        continue

    if state == 1:
        nf += l

    if '}}}' in l:
        nf += ins.read()
        nf += '\n'
        nf += '// }}}\n'
        state = 1

open(sys.argv[3],'w+').write(nf)





