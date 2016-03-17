#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import time
import hashlib
import sys
import struct


def getPIN(username):
    ''' According to code from https://github.com/nowind/sx_pi
    '''
    current_time = time.time()
    time_divided_by_five = int(current_time) // 5
    time_char = [0] * 4
    temp = [0] * 32
    time_hash = [0] * 4
    PIN27 = [0] * 6
    PIN = ""
    md5 = hashlib.md5()

    for i in xrange(0, 4):
        time_char[i] = time_divided_by_five >> (8 * (3 - i)) & 0xFF
    before_md5 = struct.pack('>I', time_divided_by_five) + \
        (username.split('@')[0] + "singlenet01").encode('ascii')
    md5.update(before_md5)
    after_md5 = md5.hexdigest()

    for i in xrange(0, 32):
        temp[i] = time_char[(31 - i) / 8] & 1
        time_char[(31 - i) / 8] = time_char[(31 - i) / 8] >> 1

    for i in xrange(0, 4):
        time_hash[i] = temp[i] * 128 + temp[4 + i] * 64 + temp[8 + i] * \
            32 + temp[12 + i] * 16 + temp[16 + i] * 8 + temp[20 + i] * \
            4 + temp[24 + i] * 2 + temp[28 + i]

    temp[1] = (time_hash[0] & 3) << 4
    temp[0] = (time_hash[0] >> 2) & 0x3F
    temp[2] = (time_hash[1] & 0xF) << 2
    temp[1] = (time_hash[1] >> 4 & 0xF) + temp[1]
    temp[3] = time_hash[2] & 0x3F
    temp[2] = ((time_hash[2] >> 6) & 0x3) + temp[2]
    temp[5] = (time_hash[3] & 3) << 4
    temp[4] = (time_hash[3] >> 2) & 0x3F

    for i in xrange(0, 6):
        PIN27[i] = temp[i] + 0x020
        if PIN27[i] >= 0x40:
            PIN27[i] += 1

    for i in xrange(0, 6):
        PIN += chr(PIN27[i])

    PIN = '\r\n' + PIN + after_md5[0] + after_md5[1] + username

    return PIN


def write_conf():
    pin = getPIN(sys.argv[1])

    with open('provider_tpl', 'r') as fp:
        template = fp.read()
    conf = template + '\"' + pin + '\"'

    with open('/etc/ppp/peers/dsl-provider', 'w') as fp:
        fp.write(conf)

    with open('pap_tpl', 'r') as fp:
        template = fp.read()
    conf = template + '\"' + getPIN(sys.argv[1]) + \
        '\" * \"' + sys.argv[2] + '\"'

    with open('/etc/ppp/pap-secrets', 'w') as fp:
        fp.write(conf)

if __name__ == '__main__':
    write_conf()
    os.system('pon dsl-provider')
