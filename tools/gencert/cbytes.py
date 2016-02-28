import fileinput

buf = bytearray()

for l in fileinput.input():
    buf = buf + bytearray(l)

print repr(buf)

a = ''.join(map(lambda c:'\\x%02x'%c, buf))
#a = str(buf).encode('string-escape')

print len(a)

print a

for i in range(0,len(a), 80):
    print "\""+a[i:i+80]+"\""
