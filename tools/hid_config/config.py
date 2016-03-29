#!/usr/bin/env python
#
#   Configures U2F Zero ATECC device if the token is
#   running the setup build.
#
#   Saves generated public key (r,s) to specified filename in ascii hex
#
import hid
import time,sys,array,binascii

class commands:
    U2F_CONFIG_GET_SERIAL_NUM = 0x80
    U2F_CONFIG_IS_BUILD = 0x81
    U2F_CONFIG_IS_CONFIGURED = 0x82
    U2F_CONFIG_LOCK = 0x83
    U2F_CONFIG_GENKEY = 0x84

if len(sys.argv) != 2:
    print 'usage: %s <public-key-output>' % sys.argv[0]
    sys.exit(1)

def open_u2f():
    h = hid.device()
    try:
        h.open(0x10c4,0x8acf)
    except IOError,ex:
        print ex
        print 'U2F Zero not found'
        return None
    return h

def die(msg):
    print msg
    sys.exit(1)


def feed_crc(crc, b):
    crc ^= b
    crc = (crc >> 1) ^ 0xa001 if crc & 1 else crc >> 1
    crc = (crc >> 1) ^ 0xa001 if crc & 1 else crc >> 1
    crc = (crc >> 1) ^ 0xa001 if crc & 1 else crc >> 1
    crc = (crc >> 1) ^ 0xa001 if crc & 1 else crc >> 1
    crc = (crc >> 1) ^ 0xa001 if crc & 1 else crc >> 1
    crc = (crc >> 1) ^ 0xa001 if crc & 1 else crc >> 1
    crc = (crc >> 1) ^ 0xa001 if crc & 1 else crc >> 1
    crc = (crc >> 1) ^ 0xa001 if crc & 1 else crc >> 1
    return crc

def reverse_bits(crc):
    crc = (((crc & 0xaaaa) >> 1) | ((crc & 0x5555) << 1))
    crc = (((crc & 0xcccc) >> 2) | ((crc & 0x3333) << 2))
    crc = (((crc & 0xf0f0) >> 4) | ((crc & 0x0f0f) << 4))
    return (((crc & 0xff00) >> 8) | ((crc & 0x00ff) << 8))

def get_crc(data):
    crc = 0
    for i in data:
        crc = feed_crc(crc,ord(i))
    crc = reverse_bits(crc)
    crc2 = crc & 0xff;
    crc1 = (crc>>8) & 0xff;
    return [crc1,crc2]


config = "\x01\x23\x6d\x10\x00\x00\x50\x00\xd7\x2c\xa5\x71\xee\xc0\x85\x00\xc0\x00\x55\x00\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x00\x00\x00\x1f\x00\x60\x00\x60\xff\xff\xff\xff\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x55\x55\xff\xff\x00\x00\x00\x00\x00\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x33\x00";

h = open_u2f()

h.write([commands.U2F_CONFIG_IS_BUILD])
data = h.read(64,1000)
if data[1] == 1:
    print 'Device is configured.'
else:
    die('Device not configured')

time.sleep(0.250)

h.write([commands.U2F_CONFIG_GET_SERIAL_NUM])
while True:
    data = h.read(64,1000)
    l = data[1]
    print 'read %i bytes' % l
    if data[0] == commands.U2F_CONFIG_GET_SERIAL_NUM:
        break
config = array.array('B',data[2:2+l]).tostring() + config[l:]
time.sleep(0.250)


crc = get_crc(config)
print 'crc is ', [hex(x) for x in crc]
h.write([commands.U2F_CONFIG_LOCK] + crc)
data = h.read(64,1000)
if data[1] == 1:
    print 'locked eeprom with crc ',crc
else:
    die('eeprom not locked')
time.sleep(0.250)

h.write([commands.U2F_CONFIG_GENKEY])
data = h.read(64,1000)
data = array.array('B',data).tostring()
data = binascii.hexlify(data)
print 'generated key:'
print data
open(sys.argv[1],'w+').write(data)


print 'Done'

