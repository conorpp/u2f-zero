import fileinput
import sys


line = sys.stdin.read()

sys.stdout.write(''.join([chr(int(''.join(c), 16)) for c in zip(line[0::2],line[1::2])]))
