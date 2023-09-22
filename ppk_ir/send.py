from scapy.all import *
import sys
import struct
from scapy.all import *
import sys, os

TYPE_GEO = 0x080A

class MyGeo(Packet, IPTools):
    name = "MyGeo"
    fields_desc = [BitField("version", 4, 4),
                   BitField("ihl", None, 4),
                   XByteField("tos", 0),
                   ShortField("len", None),
                   ShortField("id", 1),
                   FlagsField("flags", 0, 3, ["MF", "DF", "evil"]),
                   BitField("frag", 0, 13),
                   ByteField("ttl", 64),
                   ByteEnumField("proto", 0, IP_PROTOS),
                   XShortField("chksum", None),
                   # IPField("src", "127.0.0.1"),
                   Emph(SourceIPField("src", "dst")),
                   Emph(IPField("dst", "127.0.0.1"))]
    def mysummary(self):
        s = self.sprintf("%IP.src% > %IP.dst% %IP.proto%")
        if self.frag:
            s += " frag:%i" % self.frag
        return s


bind_layers(Ether, MyGeo, type=TYPE_GEO)
bind_layers(MyGeo, ICMP, frag=0, proto=1)
bind_layers(MyGeo, TCP, frag=0, proto=6)
bind_layers(MyGeo, UDP, frag=0, proto=17)

data = '123456789'
pkt2 = Ether(dst="e4:95:6e:2f:d3:44")/\
      IPv6(src='fe80::e695:6eff:fe2f:2278', dst='fe80:0000:0000:0000:e695:6eff:fe2f:d6af')\
      /UDP(sport=100,dport=200)/\
      data
pkt = Ether(src="00:00:00:00:01:03",dst="00:00:00:00:01:02")/\
      IP(src='192.168.30.181', dst='192.168.30.183')/UDP(sport=100,dport=200)

#pkt = Ether(src="a0:36:9f:a8:7f:43",dst="a0:36:9f:a8:7f:40")/\
#      IP(src='192.168.30.181', dst='192.168.30.183', tos=0xc0, ttl=60)/ICMP(type="echo-request")
pkt3 = Ether(dst="e4:95:6e:2f:d3:44")/\
      MyGeo(src='192.168.30.182', dst='192.168.30.183')\
      /UDP(sport=100,dport=200)/\
      data
pkt4 = Ether(dst="aa:bb:bb:aa:00:02")/\
      data
a1=int(input("Type:"))
a=int(input("Num:"))
for i in range(a):
	print(i)
	if a1==1:
		sendp(pkt, inter=1, count=1, iface="enp1s0f3")
		time.sleep(1)
	elif a1==2:
		sendp(pkt2, inter=1, count=1, iface="enp1s0f0")
		time.sleep(1)
	elif a1==3:
		sendp(pkt3, inter=1, count=1, iface="enp1s0f0")
		time.sleep(1)
	elif a1==4:
		sendp(pkt4, inter=1, count=1, iface="enp1s0f0")
		time.sleep(1)
