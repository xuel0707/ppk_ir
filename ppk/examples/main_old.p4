/*
 * Copyright 2019-present Open Networking Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <core.p4>
#include <v1model.p4>

// CPU_PORT specifies the P4 port number associated to controller packet-in and
// packet-out. All packets forwarded via this port will be delivered to the
// controller as P4Runtime PacketIn messages. Similarly, PacketOut messages from
// the controller will be seen by the P4 pipeline as coming from the CPU_PORT.
#define CPU_PORT 255

// CPU_CLONE_SESSION_ID specifies the mirroring session for packets to be cloned
// to the CPU port. Packets associated with this session ID will be cloned to
// the CPU_PORT as well as being transmitted via their egress port (set by the
// bridging/routing/acl table). For cloning to work, the P4Runtime controller
// needs first to insert a CloneSessionEntry that maps this session ID to the
// CPU_PORT.
#define CPU_CLONE_SESSION_ID 99


typedef bit<9>   port_num_t;
typedef bit<48>  mac_addr_t;
typedef bit<16>  mcast_group_id_t;
typedef bit<32>  ipv4_addr_t;
typedef bit<16>  l4_port_t;

const bit<8> IP_PROTO_ICMP   = 1;
const bit<8> IP_PROTO_TCP    = 6;
const bit<8> IP_PROTO_UDP    = 17;

/* Header Stuff */
enum bit<16> ether_type_t {
    TPID = 0x8100,
    IPV4 = 0x0800,
    ARP  = 0x0806,
    IPV6 = 0x86DD,
    MPLS = 0x8847,
    GEO=0x8947,
    MF=0x27C0,
    NDN=0x8624
}
const bit<16> ETHERTYPE_IPV4 =  0x0800;
const bit<16> ETHERTYPE_ARP =  0x0806;
const bit<16> ETHERTYPE_IPV6 =  0x86DD;

const bit<16> ETHERTYPE_GEO =  0x8947;
const bit<16> ETHERTYPE_MF =  0x27c0;
const bit<16> ETHERTYPE_NDN =  0x8624;
const bit<4> TYPE_geo_gbc = 0x0004;     

enum bit<16> arp_opcode_t {
    REQUEST = 1,
    REPLY   = 2
}
//------------------------------------------------------------------------------
// HEADER DEFINITIONS
//------------------------------------------------------------------------------
header ethernet_t {
    mac_addr_t  dst_addr;
    mac_addr_t  src_addr;
    ether_type_t  ether_type;
}
header mf_guid_t{
    bit<32> mf_type;
	bit<32> src_guid;
    bit<32> dest_guid;
}
header ndn_t{
    bit<16>  pad0;
	bit<128> pad1;
    bit<40>  pad2;
    bit<8>   ndnDoman;
}
header geo_t{
    bit<4>  version;
    bit<4>  nh_basic;
    bit<8>  reserved_basic;
    bit<8>  lt;
    bit<8>  rhl;
    bit<4> nh_common;
    bit<4> reserved_common_a;
    bit<4> ht;
    bit<4> hst;
    bit<8> tc;
    bit<8> flag;
    bit<16> pl;
    bit<8> mhl;
    bit<8> reserved_common_b;
}

header gbc_t{
    bit<16> sn;
    bit<16> reserved_gbc_a;
    bit<64> gnaddr;
    bit<32> tst;
    bit<32> lat;
    bit<32> longg;
    bit<1> pai;
    bit<15> s;
    bit<16> h;
    bit<32> geoAreaPosLat; //请求区域中心点的维度
    bit<32> geoAreaPosLon; //请求区域中心点的经度
    bit<16> disa;
    bit<16> disb;
    bit<16> angle;
    bit<16> reserved_gbc_b; 
}
header arp_t {
    bit<16>       hw_type;
    ether_type_t  proto_type;
    bit<8>        hw_addr_len;
    bit<8>        proto_addr_len;
    arp_opcode_t  opcode;
} 

header arp_ipv4_t {
    mac_addr_t   src_hw_addr;
    ipv4_addr_t  src_proto_addr;
    mac_addr_t   dst_hw_addr;
    ipv4_addr_t  dst_proto_addr;
}

header ipv4_t {
    bit<4>   version;
    bit<4>   ihl;
    bit<6>   dscp;
    bit<2>   ecn;
    bit<16>  total_len;
    bit<16>  identification;
    bit<3>   flags;
    bit<13>  frag_offset;
    bit<8>   ttl;
    bit<8>   protocol;
    bit<16>  hdr_checksum;
    ipv4_addr_t  src_addr;
    ipv4_addr_t  dst_addr;
}


header tcp_t {
    bit<16>  src_port;
    bit<16>  dst_port;
    bit<32>  seq_no;
    bit<32>  ack_no;
    bit<4>   data_offset;
    bit<3>   res;
    bit<3>   ecn;
    bit<6>   ctrl;
    bit<16>  window;
    bit<16>  checksum;
    bit<16>  urgent_ptr;
}

header udp_t {
    bit<16> src_port;
    bit<16> dst_port;
    bit<16> len;
    bit<16> checksum;
}

header icmp_t {
    bit<8>   type;
    bit<8>   icmp_code;
    bit<16>  checksum;
    bit<16>  identifier;
    bit<16>  sequence_number;
    bit<64>  timestamp;
}

@controller_header("packet_in")
header cpu_in_header_t {
    port_num_t  ingress_port;
    bit<7>      _pad;
}

@controller_header("packet_out")
header cpu_out_header_t {
    port_num_t  egress_port;
    bit<7>      _pad;
}

struct headers {
    cpu_out_header_t 	cpu_out;
    cpu_in_header_t 	cpu_in;
    ethernet_t 			ethernet;
    mf_guid_t mf;
    ndn_t ndn;

    gbc_t gbc;
    geo_t geo;

	arp_t       		arp;
	arp_ipv4_t 			arp_ipv4;
    ipv4_t 				ipv4;
    tcp_t 				tcp;
    udp_t 				udp;
    icmp_t 				icmp;
}

struct metadata {
    l4_port_t   l4_src_port;
    l4_port_t   l4_dst_port;
    bool        is_multicast;
    bit<8>      ip_proto;
    bit<8>      icmp_type;
	ipv4_addr_t dst_ipv4;
}


//------------------------------------------------------------------------------
// INGRESS PIPELINE
//------------------------------------------------------------------------------

parser ParserImpl (packet_in packet,
                   out headers hdr,
                   inout metadata meta,
                   inout standard_metadata_t standard_metadata)
{
    state start {
        transition select(standard_metadata.ingress_port) {
            CPU_PORT: parse_packet_out;
            default: parse_ethernet;
        }
    }

    state parse_packet_out {
        packet.extract(hdr.cpu_out);
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.ether_type){
            ether_type_t.IPV4: parse_ipv4;
			ether_type_t.ARP: parse_arp;
            ether_type_t.MF: parse_mf;
            ether_type_t.GEO: parse_geo;
            ether_type_t.NDN: parse_ndn;
            default: accept;
        }
    }
    state parse_mf{
        packet.extract(hdr.mf);
        transition accept;
    }
    state parse_ndn{
        packet.extract(hdr.ndn);
        transition accept;
    }
  
    state parse_geo{
        packet.extract(hdr.geo);
        transition select(hdr.geo.ht) { //要根据ht的大小来判断选取的字段
            TYPE_geo_gbc: parse_gbc;       //0x04
            default: accept;
        }
    }
    state parse_gbc{
        packet.extract(hdr.gbc);
        transition accept;
    }

	state parse_arp {
        packet.extract(hdr.arp);
        transition select(hdr.arp.hw_type, hdr.arp.proto_type) {
            (0x0001, ether_type_t.IPV4) : parse_arp_ipv4;
            default: reject; // Currently the same as accept
        }
    }
	
	state parse_arp_ipv4 {
        packet.extract(hdr.arp_ipv4);
        meta.dst_ipv4 = hdr.arp_ipv4.dst_proto_addr;
        transition accept;
    }

    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        meta.ip_proto = hdr.ipv4.protocol;
        transition select(hdr.ipv4.protocol) {
            IP_PROTO_TCP: parse_tcp;
            IP_PROTO_UDP: parse_udp;
            IP_PROTO_ICMP: parse_icmp;
            default: accept;
        }
    }

    state parse_tcp {
        packet.extract(hdr.tcp);
        meta.l4_src_port = hdr.tcp.src_port;
        meta.l4_dst_port = hdr.tcp.dst_port;
        transition accept;
    }

    state parse_udp {
        packet.extract(hdr.udp);
        meta.l4_src_port = hdr.udp.src_port;
        meta.l4_dst_port = hdr.udp.dst_port;
        transition accept;
    }

    state parse_icmp {
        packet.extract(hdr.icmp);
        meta.icmp_type = hdr.icmp.type;
        transition accept;
    }

}


control VerifyChecksumImpl(inout headers hdr,
                           inout metadata meta)
{
    apply { /* EMPTY */ }
}


control IngressPipeImpl (inout headers    hdr,
                         inout metadata    meta,
                         inout standard_metadata_t standard_metadata) {

    // Drop action shared by many tables.
    action drop() {
        mark_to_drop(standard_metadata);
    }

    action set_egress_port(port_num_t port_num) {
        standard_metadata.egress_spec = port_num;
    }
    action update_l2_mac(mac_addr_t target_mac) {
        hdr.ethernet.dst_addr = target_mac;
    }
	action set_egress_port_and_mac(port_num_t port_num,mac_addr_t target_mac) {
            set_egress_port(port_num);
            update_l2_mac(target_mac);
    }


	action send_arp_reply(mac_addr_t target_mac) {
        hdr.ethernet.dst_addr = hdr.arp_ipv4.src_hw_addr;
        hdr.ethernet.src_addr = target_mac;
        hdr.arp.opcode = arp_opcode_t.REPLY;
        hdr.arp_ipv4.dst_hw_addr    = hdr.arp_ipv4.src_hw_addr;
        hdr.arp_ipv4.dst_proto_addr = hdr.arp_ipv4.src_proto_addr;
        hdr.arp_ipv4.src_hw_addr    = target_mac;
        hdr.arp_ipv4.src_proto_addr = meta.dst_ipv4;
        standard_metadata.egress_spec = standard_metadata.ingress_port;
    }
	
	table arp_respond_table {
        key = {
            meta.dst_ipv4  : exact;
        }
        actions = {
            send_arp_reply;
            @defaultonly drop;
        }
        const default_action = drop;
		@name("arp_respond_table_counter")
        counters = direct_counter(CounterType.packets_and_bytes);
    }
	
	action set_multicast_group(mcast_group_id_t gid) {
        standard_metadata.mcast_grp = gid;
        meta.is_multicast = true;
    }

    table l2_exact_table {
        key = {
            hdr.ethernet.dst_addr: exact;
        }
        actions = {
            set_egress_port;
			set_multicast_group;
            @defaultonly drop;
        }
        const default_action = drop;
        @name("l2_exact_table_counter")
        counters = direct_counter(CounterType.packets_and_bytes);
    }
	
	table update_mac_table {
        key = {
            hdr.ipv4.dst_addr:	exact;
        }
        actions = {
			update_l2_mac;
            @defaultonly drop;
        }
        const default_action = drop;
        @name("update_mac_table_counter")
        counters = direct_counter(CounterType.packets_and_bytes);
    }

    action send_to_cpu() {
        standard_metadata.egress_spec = CPU_PORT;
    }

    action clone_to_cpu() {
        clone3(CloneType.I2E, CPU_CLONE_SESSION_ID, { standard_metadata.ingress_port });
    }

    table acl_table {
        key = {
            standard_metadata.ingress_port: ternary;
            hdr.ethernet.dst_addr:          ternary;
            hdr.ethernet.src_addr:          ternary;
            hdr.ethernet.ether_type:        ternary;
			hdr.ipv4.src_addr:				ternary;
			hdr.ipv4.dst_addr:				ternary;
			hdr.mf.dest_guid:				ternary;
			hdr.ndn.ndnDoman:				ternary;
			hdr.gbc.geoAreaPosLat:			ternary;
			hdr.gbc.geoAreaPosLon:			ternary;
            meta.ip_proto:        ternary;
            meta.icmp_type:       ternary;
        }
        actions = {
            send_to_cpu;
            clone_to_cpu;
			set_egress_port;
            set_egress_port_and_mac;
            drop;
        }
        @name("acl_table_counter")
        counters = direct_counter(CounterType.packets_and_bytes);
    }

    apply {
		
        if (hdr.cpu_out.isValid()) {
            standard_metadata.egress_spec = hdr.cpu_out.egress_port;
            hdr.cpu_out.setInvalid();
            exit;
        }

        bool do_l3_l2 = true;
		
		//arp request	
		if(hdr.arp.isValid() && hdr.arp_ipv4.isValid()){
			if (arp_respond_table.apply().hit) {
                do_l3_l2 = false;
            }
		}

        if (do_l3_l2) {
            if (!l2_exact_table.apply().hit) {
                update_mac_table.apply();
            }
        }
		
        acl_table.apply();
    }
}


control EgressPipeImpl (inout headers hdr,
                        inout metadata meta,
                        inout standard_metadata_t standard_metadata) {
    apply {

        if (standard_metadata.egress_port == CPU_PORT) {

            hdr.cpu_in.setValid();
            hdr.cpu_in.ingress_port = standard_metadata.ingress_port;
            exit;
        }

        if (meta.is_multicast == true &&
              standard_metadata.ingress_port == standard_metadata.egress_port) {
            mark_to_drop(standard_metadata);
        }
    }
}


control ComputeChecksumImpl(inout headers hdr,
                            inout metadata meta)
{
    apply {
    }
}


control DeparserImpl(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.cpu_in);
        packet.emit(hdr.ethernet);

        packet.emit(hdr.mf);
        packet.emit(hdr.geo);
        packet.emit(hdr.gbc);
        packet.emit(hdr.ndn);

		packet.emit(hdr.arp);
		packet.emit(hdr.arp_ipv4);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.tcp);
        packet.emit(hdr.udp);
        packet.emit(hdr.icmp);
    }
}


V1Switch(
    ParserImpl(),
    VerifyChecksumImpl(),
    IngressPipeImpl(),
    EgressPipeImpl(),
    ComputeChecksumImpl(),
    DeparserImpl()
) main;
