# Copyright 2017-present Open Networking Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import re
import socket
import struct
import math

'''
This package contains several helper functions for encoding to and decoding from byte strings:
- integers
- IPv4 address strings
- Ethernet address strings
'''

mac_pattern = re.compile('^([\da-fA-F]{2}:){5}([\da-fA-F]{2})$')
def matchesMac(mac_addr_string):
    return mac_pattern.match(mac_addr_string) is not None

def encodeMac(mac_addr_string):
    return mac_addr_string.replace(':', '').decode('hex')

def decodeMac(encoded_mac_addr):
    return ':'.join(s.encode('hex') for s in encoded_mac_addr)

ip_pattern = re.compile('^(\d{1,3}\.){3}(\d{1,3})$')
def matchesIPv4(ip_addr_string):
    return ip_pattern.match(ip_addr_string) is not None

def encodeIPv4(ip_addr_string):
    return socket.inet_aton(ip_addr_string)

def decodeIPv4(encoded_ip_addr):
    return socket.inet_ntoa(encoded_ip_addr)

ndn_parrern = re.compile('^([\da-fA-F0-9]{2}):([\da-fA-F0-9]{2})$')
def matchNdn(ndn_fib_string):
    return ndn_parrern.match(ndn_fib_string) is not None

def encodeNdn(ndn_fib_string):
    return ndn_fib_string.replace(':', '').decode('hex')

ipv6_pattern = re.compile('^((([0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){1,7}:)|(([0-9A-Fa-f]{1,4}:){6}:[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){5}(:[0-9A-Fa-f]{1,4}){1,2})|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){1,3})|(([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){1,4})|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){1,5})|([0-9A-Fa-f]{1,4}:(:[0-9A-Fa-f]{1,4}){1,6})|(:(:[0-9A-Fa-f]{1,4}){1,7})|(([0-9A-Fa-f]{1,4}:){6}(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(([0-9A-Fa-f]{1,4}:){5}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){0,1}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){0,2}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){0,3}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|([0-9A-Fa-f]{1,4}:(:[0-9A-Fa-f]{1,4}){0,4}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(:(:[0-9A-Fa-f]{1,4}){0,5}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3}))$')
def matchesIPv6(ipv6_addr_string):
    return ipv6_pattern.match(ipv6_addr_string) is not None

def encodeIPv6(ipv6_addr_string):
    return ipv6_addr_string.replace(':', '').decode('hex')
	# print("ipv6addr_type",type(ipv6_addr_string))
	# return socket.inet_aton(ipv6_addr_string)
	# ipv6_n = [socket.htonl(ipv6_addr_string[0]),
	# socket.htonl(ipv6_addr_string[1]),
	# socket.htonl(ipv6_addr_string[2]),
	# socket.htonl(ipv6_addr_string[3])]
	# data = struct.pack('IIII', ipv6_n[0], ipv6_n[1], ipv6_n[2], ipv6_n[3])
    # 	return  socket.inet_ntop(socket.AF_INET6, data) 
	# ipv6_addr_string = "\0x"+ipv6_addr_string[0]+ipv6_addr_string[1]+"\0x"+ipv6_addr_string[2]+ipv6_addr_string[3]+"\0x"+ipv6_addr_string[5]+ipv6_addr_string[6]+"\0x"+ipv6_addr_string[7]+ipv6_addr_string[8]+"\0x"+ipv6_addr_string[10]+ipv6_addr_string[11]+"\0x"+ipv6_addr_string[12]+ipv6_addr_string[13]+"\0x"+ipv6_addr_string[15]+ipv6_addr_string[16]+"\0x"+ipv6_addr_string[17]+ipv6_addr_string[18]+"\0x"+ipv6_addr_string[20]+ipv6_addr_string[21]+"\0x"+ipv6_addr_string[22]+ipv6_addr_string[23]+"\0x"+ipv6_addr_string[25]+ipv6_addr_string[26]+"\0x"+ipv6_addr_string[27]+ipv6_addr_string[28]+"\0x"+ipv6_addr_string[30]+ipv6_addr_string[31]+"\0x"+ipv6_addr_string[32]+ipv6_addr_string[33]+"\0x"+ipv6_addr_string[35]+ipv6_addr_string[36]+"\0x"+ipv6_addr_string[37]+ipv6_addr_string[38]

	# return ipv6_addr_string


# def decodeIPv6(encoded_ipv6_addr):
#     return 


def bitwidthToBytes(bitwidth):
    return int(math.ceil(bitwidth / 8.0))

def encodeNum(number, bitwidth):
    byte_len = bitwidthToBytes(bitwidth)
    num_str = '%x' % number
    if number >= 2 ** bitwidth:
        raise Exception("Number, %d, does not fit in %d bits" % (number, bitwidth))
    return ('0' * (byte_len * 2 - len(num_str)) + num_str).decode('hex')

def decodeNum(encoded_number):
    return int(encoded_number.encode('hex'), 16)

def encode(x, bitwidth):
    'Tries to infer the type of `x` and encode it'
    byte_len = bitwidthToBytes(bitwidth)
    # print(len(x))
    # print("x_type",type(x))
    if (type(x) == list or type(x) == tuple) and len(x) == 1:
        print("xxxx")
        x = x[0]
    encoded_bytes = None
    if type(x) == str:
        print(x)
        if matchesMac(x):
            encoded_bytes = encodeMac(x)
        elif matchesIPv4(x):
            encoded_bytes = encodeIPv4(x)
        elif matchesIPv6(x):
	    print("IPv6----------")
            encoded_bytes = encodeIPv6(x)
        elif matchNdn(x):
            encoded_bytes = encodeNdn(x)
            print(1)
	    # print("str:%s",encoded_bytes)
        else:
            # Assume that the string is already encoded
            encoded_bytes = x
        print(1)
    elif type(x) == int:
        print(2)
        encoded_bytes = encodeNum(x, bitwidth)
    else:
        raise Exception("Encoding objects of %r is not supported" % type(x))
    print("encoded_bytes",len(encoded_bytes))
    print("byte_len",byte_len )
    assert(len(encoded_bytes) == byte_len)
    # print(len(encoded_bytes),byte_len)
    # print(encoded_bytes,byte_len)
    return encoded_bytes

if __name__ == '__main__':
    # TODO These tests should be moved out of main eventually
    # mac = "aa:bb:cc:dd:ee:ff"
    # enc_mac = encodeMac(mac)
    # assert(enc_mac == '\xaa\xbb\xcc\xdd\xee\xff')
    # dec_mac = decodeMac(enc_mac)
    # assert(mac == dec_mac)

    # ip = "10.0.0.1"
    # enc_ip = encodeIPv4(ip)
    # assert(enc_ip == '\x0a\x00\x00\x01')
    # dec_ip = decodeIPv4(enc_ip)
    # assert(ip == dec_ip)

    # # ipv6 = "fe80:0000:0000:0000:e695:6eff:fe2f:d6af"
    # # enc_ipv6 = encodeIPv6(ipv6)
    # # assert()

    # num = 1337
    # byte_len = 5
    # enc_num = encodeNum(num, byte_len * 8)
    # assert(enc_num == '\x00\x00\x00\x05\x39')
    # dec_num = decodeNum(enc_num)
    # assert(num == dec_num)

    # assert(matchesIPv4('10.0.0.1'))
    # assert(not matchesIPv4('10.0.0.1.5'))
    # assert(not matchesIPv4('1000.0.0.1'))
    # assert(not matchesIPv4('10001'))

    # assert(encode(mac, 6 * 8) == enc_mac)
    # assert(encode(ip, 4 * 8) == enc_ip)
    # assert(encode(num, 5 * 8) == enc_num)
    # assert(encode((num,), 5 * 8) == enc_num)
    # assert(encode([num], 5 * 8) == enc_num)

    num = 256
    byte_len = 2
    try:
        enc_num = encodeNum(num, 8)
        raise Exception("expected exception")
    except Exception as e:
        print e
