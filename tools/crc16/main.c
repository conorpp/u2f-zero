
#include <stdio.h>
#include <stdint.h>

uint16_t feed_crc(uint16_t crc, uint8_t b)
{
    crc ^= b;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    return crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
}

uint16_t reverse_bits(uint16_t crc)
{
    // efficient bit reversal for 16 bit int
    crc = (((crc & 0xaaaa) >> 1) | ((crc & 0x5555) << 1));
    crc = (((crc & 0xcccc) >> 2) | ((crc & 0x3333) << 2));
    crc = (((crc & 0xf0f0) >> 4) | ((crc & 0x0f0f) << 4));
    return (((crc & 0xff00) >> 8) | ((crc & 0x00ff) << 8));
}

uint8_t d [] = "\x01\x23\x0c\x82\x00\x00\x50\x00\xf5\x39\x85\xef\xee\xc0\x65\x00\xc0\x00\x55\x00\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\x83\xa0\xff\xff\xff\xff\x00\x00\x00\x00\x00\x0f\xff\xff\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x55\x55\xff\xff\x00\x00\x00\x00\x00\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x13\x00\x33\x00\x33\x00\x33\x00\x33\x00\x33\x00\x33\x00\x33\x00\x33\x00";


int main()
{
    uint16_t crc = 0;
    int i;
    for (i=0; i< sizeof(d)-1; i++)
    {
        crc = feed_crc(crc,d[i]);
    }
    printf("crc: %hx\n",reverse_bits(crc));
    return 0;
}
