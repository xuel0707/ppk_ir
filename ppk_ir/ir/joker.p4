#include <core.p4>
#define V1MODEL_VERSION 20180101
#include <v1model.p4>

header ethernet_t {
    bit<48> dstAddr;
    bit<48> srcAddr;
    bit<16> ether_type;
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
    ethernet_t ethernet;
    mf_guid_t  mf;
    geo_t      geo;
    gbc_t      gbc;
    beacon_t   beacon;
    btp_t      btp;
    its_t      its;
}

struct metadata {
}

parser ParserImpl(packet_in packet, out headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name(".start") state start {
        transition parse_ethernet;
    }
    @name(".parse_ethernet") state parse_ethernet {
        packet.extract<ethernet_t>(hdr.ethernet);
        transition select(hdr.ethernet.ether_type) {
            16w0x27c0: parse_mf;
            16w0x8947: parse_geo;
            default: accept;
        }
    }
    @name(".parse_mf") state parse_mf {
        packet.extract<mf_guid_t>(hdr.mf);
        transition accept;
    }
    @name(".parse_geo") state parse_geo {
        packet.extract<geo_t>(hdr.geo);
        transition select(hdr.geo.ht) {
            4w0x1: parse_beacon;
            4w0x4: parse_gbc;
            4w0x5: parse_tsb;
            default: accept;
        }
    }
    @name(".parse_beacon") state parse_beacon {
        packet.extract<beacon_t>(hdr.beacon);
        transition accept;
    }
    @name(".parse_gbc") state parse_gbc {
        packet.extract<gbc_t>(hdr.gbc);
        transition parse_btp;
    }
    @name(".parse_btp") state parse_btp {
        packet.extract<btp_t>(hdr.btp);
        transition parse_its;
    }
    @name(".parse_its") state parse_its {
        packet.extract<its_t>(hdr.its);
        transition accept;
    }
    @name(".parse_tsb") state parse_tsb {
        transition accept;
    }
}

control IngressImpl(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name("IngressImpl.mf_set_egress_port") action mf_set_egress_port(@name("port_num") bit<9> port_num) {
        standard_metadata.egress_spec = port_num;
    }
    @name("IngressImpl.mf_drop") action mf_drop() {
        mark_to_drop(standard_metadata);
    }
    @name("IngressImpl.mf_exact") table mf_exact_0 {
        key = {
            hdr.mf.dest_guid: exact @name("hdr.mf.dest_guid");
        }
        actions = {
            mf_set_egress_port();
            mf_drop();
        }
        size = 1024;
        default_action = mf_drop();
    }
    @name("IngressImpl.geo_set_egress_port") action geo_set_egress_port(@name("port_num") bit<9> port_num_5) {
        standard_metadata.egress_spec = port_num_5;
    }
    @name("IngressImpl.geo_drop") action geo_drop() {
        mark_to_drop(standard_metadata);
    }
    @name("IngressImpl.geo_exact") table geo_exact_0 {
        key = {
            hdr.gbc.geoAreaPosLat: exact @name("hdr.gbc.geoAreaPosLat");
            hdr.gbc.geoAreaPosLon: exact @name("hdr.gbc.geoAreaPosLon");
            hdr.gbc.disa         : exact @name("hdr.gbc.disa");
            hdr.gbc.disb         : exact @name("hdr.gbc.disb");
        }
        actions = {
            geo_set_egress_port();
            geo_drop();
        }
        size = 1024;
        default_action = geo_drop();
    }
    apply {
        if (hdr.mf.isValid()) {
            mf_exact_0.apply();
        }
        if (hdr.geo.isValid()) {
            geo_exact_0.apply();
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
        packet.emit<mf_guid_t>(hdr.mf);
        packet.emit<geo_t>(hdr.geo);
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

V1Switch<headers, metadata>(ParserImpl(), verifyChecksum(), IngressImpl(), EgressPipeImpl(), computeChecksum(), DeparserImpl()) main;
