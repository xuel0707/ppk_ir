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

// Maximum number of hops supported when using SRv6.
// Required for Exercise 7.
#define SRV6_MAX_HOPS 4

typedef bit<9>   port_num_t;

typedef bit<48>  mac_addr_t;
typedef bit<16>  mcast_group_id_t;
typedef bit<32>  ipv4_addr_t;
typedef bit<128> ipv6_addr_t;
typedef bit<16>  l4_port_t;


const bit<16> ETHERTYPE_TAG =  0xfe00;
const bit<16> ETHERTYPE_IPV4 = 0x0800;
const bit<16> ETHERTYPE_IPV6 = 0x86dd;
const bit<16> ETHERTYPE_GEO =  0x8947;
const bit<16> ETHERTYPE_MF =  0x27c0;
const bit<16> ETHERTYPE_NDN =  0x8624;
const bit<16> ETHERTYPE_ARP =  0x806;

const bit<4> TYPE_geo_gbc = 0x0004;     


const bit<8> IP_PROTO_ICMP   = 1;
const bit<8> IP_PROTO_TCP    = 6;
const bit<8> IP_PROTO_UDP    = 17;
const bit<8> IP_PROTO_SRV6   = 43;
const bit<8> IP_PROTO_ICMPV6 = 58;

const mac_addr_t IPV6_MCAST_01 = 0x33_33_00_00_00_01;

const bit<8> ICMP6_TYPE_NS = 135;
const bit<8> ICMP6_TYPE_NA = 136;

const bit<8> NDP_OPT_TARGET_LL_ADDR = 2;

const bit<32> NDP_FLAG_ROUTER    = 0x80000000;
const bit<32> NDP_FLAG_SOLICITED = 0x40000000;
const bit<32> NDP_FLAG_OVERRIDE  = 0x20000000;


//------------------------------------------------------------------------------
// HEADER DEFINITIONS
//------------------------------------------------------------------------------

header ethernet_t {
    mac_addr_t  dst_addr;
    mac_addr_t  src_addr;
    bit<16>     ether_type;
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
    bit<32>  src_addr;
    bit<32>  dst_addr;
}
header arp_t {
    bit <16> hrd;
    bit <16> pro;
    bit <8> hln;
    bit <8> pln;
    bit <16> op;
    mac_addr_t sha;
    ipv4_addr_t tpa;
    mac_addr_t tha;
    ipv4_addr_t spa;
}
header ipv6_t {
    bit<4>    version;
    bit<8>    traffic_class;
    bit<20>   flow_label;
    bit<16>   payload_len;
    bit<8>    next_hdr;
    bit<8>    hop_limit;
    bit<128>  src_addr;
    bit<128>  dst_addr;
}

header srv6h_t {
    bit<8>   next_hdr;
    bit<8>   hdr_ext_len;
    bit<8>   routing_type;
    bit<8>   segment_left;
    bit<8>   last_entry;
    bit<8>   flags;
    bit<16>  tag;
}

header srv6_list_t {
    bit<128>  segment_id;
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

header icmpv6_t {
    bit<8>   type;
    bit<8>   code;
    bit<16>  checksum;
}

header ndp_t {
    bit<32>      flags;
    ipv6_addr_t  target_ipv6_addr;
    // NDP option.
    bit<8>       type;
    bit<8>       length;
    bit<48>      target_mac_addr;
}
header tag_t {
    bit<16>    ether_type;
    bit<4>     domain_num;
    bit<12>     code;

}
// Packet-in header. Prepended to packets sent to the CPU_PORT and used by the
// P4Runtime server (Stratum) to populate the PacketIn message metadata fields.
// Here we use it to carry the original ingress port where the packet was
// received.
@controller_header("packet_in")
header cpu_in_header_t {
    port_num_t  ingress_port;
    bit<7>      _pad;
}

// Packet-out header. Prepended to packets received from the CPU_PORT. Fields of
// this header are populated by the P4Runtime server based on the P4Runtime
// PacketOut metadata fields. Here we use it to inform the P4 pipeline on which
// port this packet-out should be transmitted.
@controller_header("packet_out")
header cpu_out_header_t {
    port_num_t  egress_port;
    bit<7>      _pad;
}

struct headers {
    cpu_out_header_t cpu_out;
    cpu_in_header_t cpu_in;
    ethernet_t ethernet;
    arp_t arp;
    tag_t  tag;
    ipv4_t ipv4;
    ipv6_t ipv6;
    mf_guid_t mf;
    ndn_t ndn;

    gbc_t gbc;
    geo_t geo;

    tcp_t tcp;
    udp_t udp;
    icmp_t icmp;
    icmpv6_t icmpv6;
    ndp_t ndp;
}

struct metadata {
    l4_port_t   l4_src_port;
    l4_port_t   l4_dst_port;
    bool        is_multicast;
    bit<8>      ip_proto;
    bit<8>      icmp_type;

    bit<16>     ether_type;
    bit<4>      domain_num;
    bool        istag;
    bit<128>    dst_addr;
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
        meta.istag=false;
        meta.ether_type=hdr.ethernet.ether_type;

        transition select(hdr.ethernet.ether_type){
            ETHERTYPE_TAG:  parse_tag;
            ETHERTYPE_ARP: parse_arp;
            ETHERTYPE_IPV4: parse_ipv4;
            ETHERTYPE_IPV6: parse_ipv6;
            ETHERTYPE_MF: parse_mf;
            ETHERTYPE_GEO: parse_geo;
            ETHERTYPE_NDN: parse_ndn;
            default: accept;
        }
    }
    state parse_tag {
        packet.extract(hdr.tag);
        meta.istag=true;
        meta.domain_num=hdr.tag.domain_num;
        transition select(hdr.tag.ether_type){
            ETHERTYPE_IPV4: parse_ipv4;
            ETHERTYPE_IPV6: parse_ipv6;
            ETHERTYPE_MF: parse_mf;
            ETHERTYPE_GEO: parse_geo;
            ETHERTYPE_NDN: accept;
            default: accept;
        }
    }
    state parse_mf{
        packet.extract(hdr.mf);
        transition accept;
    }
    state parse_arp{
        packet.extract(hdr.arp);
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

    state parse_ipv6 {
        packet.extract(hdr.ipv6);
        meta.ip_proto =hdr.ipv6.next_hdr;
        transition select(hdr.ipv6.next_hdr) {
            IP_PROTO_TCP: parse_tcp;
            IP_PROTO_UDP: parse_udp;
            IP_PROTO_ICMPV6: parse_icmpv6;
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

    state parse_icmpv6 {
        packet.extract(hdr.icmpv6);
        meta.icmp_type = hdr.icmpv6.type;
        transition select(hdr.icmpv6.type) {
            ICMP6_TYPE_NS: parse_ndp;
            ICMP6_TYPE_NA: parse_ndp;
            default: accept;
        }
    }

    state parse_ndp {
        packet.extract(hdr.ndp);
        transition accept;
    }
}


control VerifyChecksumImpl(inout headers hdr,
                           inout metadata meta)
{
    // Not used here. We assume all packets have valid checksum, if not, we let
    // the end hosts detect errors.
    apply { /* EMPTY */ }
}


control IngressPipeImpl (inout headers    hdr,
                        inout metadata    meta,
                        inout standard_metadata_t standard_metadata) {
    direct_counter(CounterType.packets_and_bytes) nop_counter;
    direct_counter(CounterType.packets_and_bytes) in_port_counter;
    direct_counter(CounterType.packets_and_bytes) out_port_counter;

    direct_counter(CounterType.packets_and_bytes) tag_table_counter;
    direct_counter(CounterType.packets_and_bytes) doman_router_table_counter;

    // Drop action shared by many tables.
    action drop() {
        mark_to_drop(standard_metadata);
    }

    action set_egress_port(port_num_t port_num) {
        standard_metadata.egress_spec = port_num;
    }
    action set_egress_port_and_remove_tag(port_num_t port_num) {
        hdr.ethernet.ether_type = hdr.tag.ether_type;
        hdr.tag.setInvalid();
        set_egress_port(port_num);
    }
    action insert_tag_header(bit<4> domain_num) {
        hdr.tag.setValid();
        hdr.tag.ether_type = hdr.ethernet.ether_type;
        hdr.tag.domain_num = domain_num;
        hdr.tag.code = 0xfff;
        hdr.ethernet.ether_type = ETHERTYPE_TAG;
        meta.domain_num=domain_num;
        meta.istag=true;
    }
    action set_mcast_group_id(mcast_group_id_t group_id) {
        // gid will be used by the Packet Replication Engine (PRE) in the
        // Traffic Manager--located right after the ingress pipeline, to
        // replicate a packet to multiple egress ports, specified by the control
        // plane by means of P4Runtime MulticastGroupEntry messages.
        standard_metadata.mcast_grp = group_id;
        meta.is_multicast = true;
    }
    action set_mcast_group_id_and_remove_tag(mcast_group_id_t group_id) {
        hdr.ethernet.ether_type = hdr.tag.ether_type;
        hdr.tag.setInvalid();
        set_mcast_group_id(group_id);
    }

    table tag_table {
        key = {
            hdr.mf.dest_guid:       ternary;
            hdr.gbc.geoAreaPosLon:  ternary;
            hdr.ipv4.dst_addr:      ternary;
            hdr.arp.spa:            ternary;
            hdr.ndn.ndnDoman:       ternary;
            meta.ether_type:        ternary;
            //meta.dst_addr: ternary;
            standard_metadata.ingress_port: ternary;
        }
        actions = {
            insert_tag_header;
        }
        counters = tag_table_counter;
    }

    table doman_router_table {
        key = {
            hdr.tag.domain_num:ternary;
        }
        actions = {
            set_egress_port;
            set_egress_port_and_remove_tag;
            set_mcast_group_id;
            set_mcast_group_id_and_remove_tag;
        }
        counters = doman_router_table_counter;
    }

    table count_table {
        key = {
            standard_metadata.ingress_port: ternary;
            hdr.ethernet.ether_type:        ternary;
            meta.ether_type: ternary;
        }

        actions = {
            NoAction;
        }
        size = 100;
        counters = nop_counter;
    }

    table in_port_counter_table {
        key = {
            standard_metadata.ingress_port: exact;
        }
        size = 100;
        actions = {
            NoAction;
        }
        counters = in_port_counter;
    }

    table out_port_counter_table {
        key = {
            standard_metadata.egress_spec: exact;
        }
        size = 100;

        actions = {
            NoAction;
        }
        counters = out_port_counter;
    }

    apply {
        if (meta.istag){
            meta.ether_type=hdr.tag.ether_type;
        }

        in_port_counter_table.apply();

        if (hdr.cpu_out.isValid()) {
            standard_metadata.egress_spec = hdr.cpu_out.egress_port;
            hdr.cpu_out.setInvalid();
        }else{
            if (!hdr.tag.isValid()){
                tag_table.apply();
            }
            if (hdr.tag.isValid()){
                doman_router_table.apply();
            }
        }
        count_table.apply();
        out_port_counter_table.apply();
    }
}


control EgressPipeImpl (inout headers hdr,
                        inout metadata meta,
                        inout standard_metadata_t standard_metadata) {
    apply {
        if (standard_metadata.egress_port == CPU_PORT) {
            // *** TODO EXERCISE 4
            // Implement logic such that if the packet is to be forwarded to the
            // CPU port, e.g., if in ingress we matched on the ACL table with
            // action send/clone_to_cpu...
            // 1. Set cpu_in header as valid
            // 2. Set the cpu_in.ingress_port field to the original packet's
            //    ingress port (standard_metadata.ingress_port).

            hdr.cpu_in.setValid();
            hdr.cpu_in.ingress_port = standard_metadata.ingress_port;
            exit;
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
        packet.emit(hdr.tag);
        packet.emit(hdr.arp);

        packet.emit(hdr.mf);
        packet.emit(hdr.geo);
        packet.emit(hdr.gbc);
        packet.emit(hdr.ndn);

        packet.emit(hdr.ipv4);
        packet.emit(hdr.ipv6);
        packet.emit(hdr.tcp);
        packet.emit(hdr.udp);
        packet.emit(hdr.icmp);
        packet.emit(hdr.icmpv6);
        packet.emit(hdr.ndp);
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
