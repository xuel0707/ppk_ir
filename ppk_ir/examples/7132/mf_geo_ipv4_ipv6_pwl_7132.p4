/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

typedef bit<9> port_t;
typedef bit<9> port_num_t;
typedef bit<32> ipv4_addr_t;

const bit<16> TYPE_IPV4 = 0x800;
const bit<16> TYPE_IPV6 = 0x86dd;
const bit<16> TYPE_PWL = 0x88ab;
const bit<16> TYPE_MF = 0x27C0;
const bit<16> TYPE_GEO = 0x8947;
const bit<4> TYPE_geo_beacon = 0x0001;
const bit<4> TYPE_geo_gbc = 0x0004;     
const bit<4> TYPE_geo_tsb = 0x0005; 



/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

header ethernet_t {
    bit<48> dstAddr;
    bit<48> srcAddr;
    bit<16> etherType;
}


header mf_guid_t{
    bit<32> mf_type;
	bit<32> src_guid;
    bit<32> dest_guid;
}
header ipv4_t {
    bit<4>      version;
    bit<4>      ihl;
    bit<6>      dscp;
    bit<2>      ecn;
    bit<16>     total_len;
    bit<16>     identification;
    bit<3>      flags;
    bit<13>     frag_offset;
    bit<8>      ttl;
    bit<8>      protocol;
    bit<16>     hdr_checksum;
    ipv4_addr_t src_addr;
    ipv4_addr_t dst_addr;
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

header powerlink_t {
    bit<1>    saved;
    bit<7>    message_type;
    bit<8>    dst_node;
    bit<8>    src_node;
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
    bit<32> geoAreaPosLat; 
    bit<32> geoAreaPosLon; 
    bit<16> disa;
    bit<16> disb;
    bit<16> angle;
    bit<16> reserved_gbc_b; 
}


header btp_t{
    bit<16> dstport;
    bit<16> dstportinfo;
}

header its_t{
    bit<8> protoversion;
    bit<8> messid;
    bit<32> stationid;
}
header beacon_t{
    bit<64> gnaddr;
    bit<32> tst;
    bit<32> lat;
    bit<32> longg;
    bit<1> pai;
    bit<15> s;
    bit<16> h;
}


struct headers{
    ethernet_t ethernet;
    ipv4_t   ipv4;
    ipv6_t   ipv6;
    mf_guid_t mf;
    powerlink_t pwl;
    geo_t geo;
    gbc_t gbc;
    beacon_t beacon;
    btp_t btp;
    its_t its;

}

struct metadata {
    /* empty */
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser ParserImpl(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    @name(".start") state start {
        transition parse_ethernet;     
    }
    @name(".parse_ethernet") state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.etherType) { 
            TYPE_MF: parse_mf;
            TYPE_IPV4: parse_ipv4;
            TYPE_IPV6: parse_ipv6;
            TYPE_GEO: parse_geo;
            TYPE_PWL: parse_pwl;
            default: accept;
        }
    }

    @name(".parse_mf") state parse_mf{
        packet.extract(hdr.mf);
		transition accept;
    }
    @name(".parse_ipv4") state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition accept;
    }
    @name(".parse_ipv6") state parse_ipv6 {
        packet.extract(hdr.ipv6);
        transition accept;
    }
    @name(".parse_pwl") state parse_pwl{
        packet.extract(hdr.pwl);
        transition accept;
    }

    @name(".parse_geo") state parse_geo{
        packet.extract(hdr.geo);
        transition select(hdr.geo.ht) { 
            TYPE_geo_beacon: parse_beacon; 
            TYPE_geo_gbc: parse_gbc;       
            TYPE_geo_tsb: parse_tsb;    
            default: accept;
        }
    }

    @name(".parse_beacon") state parse_beacon{
        packet.extract(hdr.beacon);
        transition accept;
    }

    @name(".parse_gbc") state parse_gbc{
        packet.extract(hdr.gbc);
        transition parse_btp;
    }

    @name(".parse_btp") state parse_btp{
        packet.extract(hdr.btp);
        transition parse_its;
    }
    
    @name(".parse_its") state parse_its{
        packet.extract(hdr.its);
        transition accept;
    }

    @name(".parse_tsb") state parse_tsb{
        //packet.extract(hdr.tsb);
        transition accept;
    }
    
}




/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control IngressPipeImpl(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action mf_set_egress_port(port_num_t port_num) {
        standard_metadata.egress_spec = port_num;
    }
    action mf_drop() {
        mark_to_drop(standard_metadata);
    }

    table mf_exact {
        key = {
            hdr.mf.dest_guid: exact;
        }
        actions = {
            mf_set_egress_port;
            mf_drop;
        }
        size = 1024;
        default_action = mf_drop();
    }

    action geo_set_egress_port(port_num_t port_num) {
        standard_metadata.egress_spec = port_num;
    }
    action geo_drop() {
        mark_to_drop(standard_metadata);
    }

    table geo_exact {
        key = {
            hdr.gbc.geoAreaPosLat: exact;
            hdr.gbc.geoAreaPosLon: exact;
            hdr.gbc.disa: exact;
            hdr.gbc.disb: exact;
            hdr.its.stationid: exact;

        }
        actions = {
            geo_set_egress_port;
            geo_drop;
        }
        size = 1024;
        default_action = geo_drop();
    }

    action ipv4_set_egress_port(port_num_t port_num) {
        standard_metadata.egress_spec = port_num;
    }
    action ipv4_drop() {
        mark_to_drop(standard_metadata);
    }
    table ipv4_exact {
        key = {
            hdr.ipv4.dst_addr            : exact;
        }
        actions = {
            ipv4_set_egress_port;
            ipv4_drop;
        }
    }

    action ipv6_set_egress_port(port_num_t port_num) {
        standard_metadata.egress_spec = port_num;
    }
    action ipv6_drop() {
        mark_to_drop(standard_metadata);
    }
    table ipv6_exact {
        key = {
            hdr.ipv6.src_addr             : exact;
        }
        actions = {
            ipv6_set_egress_port;
            ipv6_drop;
        }
    }
    action pwl_set_egress_port(port_num_t port_num) {
        standard_metadata.egress_spec = port_num;
    }
    action pwl_drop() {
        mark_to_drop(standard_metadata);
    }

    table pwl_exact {
        key = {
            hdr.ethernet.dstAddr: exact;
        }
        actions = {
            pwl_set_egress_port;
            pwl_drop;
        }
        size = 1024;
        default_action = pwl_drop();
    }
		
    apply {
            if(hdr.mf.isValid()) {
				mf_exact.apply();
			}
            if(hdr.geo.isValid()) {
                geo_exact.apply();
            }
            if(hdr.ipv4.isValid()){
                ipv4_exact.apply();
            }
            if(hdr.ipv6.isValid()){
                ipv6_exact.apply();
            }
            if(hdr.pwl.isValid()){
                pwl_exact.apply();
            }         
		}
}

control EgressPipeImpl(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    apply {
    }
}



/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control DeparserImpl(packet_out packet, in headers hdr) {

        apply{
            packet.emit(hdr.ethernet);
			packet.emit(hdr.mf);
            packet.emit(hdr.geo);
            packet.emit(hdr.ipv4);
            packet.emit(hdr.ipv6);
            packet.emit(hdr.pwl);

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
/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
	ParserImpl(),
	verifyChecksum(),
	IngressPipeImpl(),
	EgressPipeImpl(),
	computeChecksum(),
	DeparserImpl()
) main;
