import socket
import struct

def encodeIPv6(ipv6_string):
	data = socket.inet_pton(socket.AF_INET6, ipv6_string)
	ipv6_n = struct.unpack('IIII', data)
	ipv6 = (socket.ntohl(ipv6_n[0]),
            socket.ntohl(ipv6_n[1]),
            socket.ntohl(ipv6_n[2]),
            socket.ntohl(ipv6_n[3]))
	ipv6_n = (socket.htonl(ipv6[0]),
              socket.htonl(ipv6[1]),
              socket.htonl(ipv6[2]),
              socket.htonl(ipv6[3]))
	data1 = struct.pack('IIII', ipv6_n[0], ipv6_n[1], ipv6_n[2], ipv6_n[3])
	ipv6 = socket.inet_ntop(socket.AF_INET6, data1)
	return ipv6	

if __name__=='__main__':
	str = "fe80:1111:0000:0000:e695:6eff:fe2f:d6af"
	print(encodeIPv6(str))
