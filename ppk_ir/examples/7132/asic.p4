#include <core.p4>
#define V1MODEL_VERSION 20180101
#include <v1model.p4>

header ethernet_t {
    bit<48> dstAddr;
    bit<48> srcAddr;
    bit<16> etherType;
}

header mf_guid_t {
    bit<32> mf_type;
    bit<32> src_guid;
    bit<32> dest_guid;
}

header ipv4_t {
    bit<4>  version;
    bit<4>  ihl;
    bit<6>  dscp;
    bit<2>  ecn;
    bit<16> total_len;
    bit<16> identification;
    bit<3>  flags;
    bit<13> frag_offset;
    bit<8>  ttl;
    bit<8>  protocol;
    bit<16> hdr_checksum;
    bit<32> src_addr;
    bit<32> dst_addr;
}

header ipv6_t {
    bit<4>   version;
    bit<8>   traffic_class;
    bit<20>  flow_label;
    bit<16>  payload_len;
    bit<8>   next_hdr;
    bit<8>   hop_limit;
    bit<128> src_addr;
    bit<128> dst_addr;
}

header powerlink_t {
    bit<1> saved;
    bit<7> message_type;
    bit<8> dst_node;
    bit<8> src_node;
}

header geo_t {
    bit<4>  version;
    bit<4>  nh_basic;
    bit<8>  reserved_basic;
    bit<8>  lt;
    bit<8>  rhl;
    bit<4>  nh_common;
    bit<4>  reserved_common_a;
    bit<4>  ht;
    bit<4>  hst;
    bit<8>  tc;
    bit<8>  flag;
    bit<16> pl;
    bit<8>  mhl;
    bit<8>  reserved_common_b;
}

header gbc_t {
    bit<16> sn;
    bit<16> reserved_gbc_a;
    bit<64> gnaddr;
    bit<32> tst;
    bit<32> lat;
    bit<32> longg;
    bit<1>  pai;
    bit<15> s;
    bit<16> h;
    bit<32> geoAreaPosLat;
    bit<32> geoAreaPosLon;
    bit<16> disa;
    bit<16> disb;
    bit<16> angle;
    bit<16> reserved_gbc_b;
}

header btp_t {
    bit<16> dstport;
    bit<16> dstportinfo;
}

header its_t {
    bit<8>  protoversion;
    bit<8>  messid;
    bit<32> stationid;
}

header beacon_t {
    bit<64> gnaddr;
    bit<32> tst;
    bit<32> lat;
    bit<32> longg;
    bit<1>  pai;
    bit<15> s;
    bit<16> h;
}

struct headers {
    ethernet_t  ethernet;
    ipv4_t      ipv4;
    ipv6_t      ipv6;
    powerlink_t pwl;
}

struct metadata {
}

parser ParserImpl(packet_in packet, out headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name(".start") state start {
        transition parse_ethernet;
    }
    @name(".parse_ethernet") state parse_ethernet {
        packet.extract<ethernet_t>(hdr.ethernet);
        transition select(hdr.ethernet.etherType) {
            16w0x800: parse_ipv4;
            16w0x86dd: parse_ipv6;
            16w0x88ab: parse_pwl;
            default: accept;
        }
    }
    @name(".parse_ipv4") state parse_ipv4 {
        packet.extract<ipv4_t>(hdr.ipv4);
        transition accept;
    }
    @name(".parse_ipv6") state parse_ipv6 {
        packet.extract<ipv6_t>(hdr.ipv6);
        transition accept;
    }
    @name(".parse_pwl") state parse_pwl {
        packet.extract<powerlink_t>(hdr.pwl);
        transition accept;
    }
}

control IngressPipeImpl(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @noWarn("unused") @name(".NoAction") action NoAction_1() {
    }
    @noWarn("unused") @name(".NoAction") action NoAction_2() {
    }
    @name("IngressPipeImpl.ipv4_set_egress_port") action ipv4_set_egress_port(@name("port_num") bit<9> port_num_6) {
        standard_metadata.egress_spec = port_num_6;
    }
    @name("IngressPipeImpl.ipv4_drop") action ipv4_drop() {
        mark_to_drop(standard_metadata);
    }
    @name("IngressPipeImpl.ipv4_exact") table ipv4_exact_0 {
        key = {
            hdr.ipv4.dst_addr: exact @name("hdr.ipv4.dst_addr");
        }
        actions = {
            ipv4_set_egress_port();
            ipv4_drop();
            @defaultonly NoAction_1();
        }
        default_action = NoAction_1();
    }
    @name("IngressPipeImpl.ipv6_set_egress_port") action ipv6_set_egress_port(@name("port_num") bit<9> port_num_7) {
        standard_metadata.egress_spec = port_num_7;
    }
    @name("IngressPipeImpl.ipv6_drop") action ipv6_drop() {
        mark_to_drop(standard_metadata);
    }
    @name("IngressPipeImpl.ipv6_exact") table ipv6_exact_0 {
        key = {
            hdr.ipv6.src_addr: exact @name("hdr.ipv6.src_addr");
        }
        actions = {
            ipv6_set_egress_port();
            ipv6_drop();
            @defaultonly NoAction_2();
        }
        default_action = NoAction_2();
    }
    @name("IngressPipeImpl.pwl_set_egress_port") action pwl_set_egress_port(@name("port_num") bit<9> port_num_8) {
        standard_metadata.egress_spec = port_num_8;
    }
    @name("IngressPipeImpl.pwl_drop") action pwl_drop() {
        mark_to_drop(standard_metadata);
    }
    @name("IngressPipeImpl.pwl_exact") table pwl_exact_0 {
        key = {
            hdr.ethernet.dstAddr: exact @name("hdr.ethernet.dstAddr");
        }
        actions = {
            pwl_set_egress_port();
            pwl_drop();
        }
        size = 1024;
        default_action = pwl_drop();
    }
    apply {
        if (hdr.ipv4.isValid()) {
            ipv4_exact_0.apply();
        }
        if (hdr.ipv6.isValid()) {
            ipv6_exact_0.apply();
        }
        if (hdr.pwl.isValid()) {
            pwl_exact_0.apply();
        }
    }
}

control EgressPipeImpl(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    apply {
    }
}

control DeparserImpl(packet_out packet, in headers hdr) {
    apply {
        packet.emit<ethernet_t>(hdr.ethernet);
        packet.emit<ipv4_t>(hdr.ipv4);
        packet.emit<ipv6_t>(hdr.ipv6);
        packet.emit<powerlink_t>(hdr.pwl);
    }
}

control verifyChecksum(inout headers hdr, inout metadata meta) {
    apply {
    }
}

control computeChecksum(inout headers hdr, inout metadata meta) {
    apply {
    }
}

V1Switch<headers, metadata>(ParserImpl(), verifyChecksum(), IngressPipeImpl(), EgressPipeImpl(), computeChecksum(), DeparserImpl()) main;
