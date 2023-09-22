/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

typedef bit<9> port_t;
typedef bit<9> port_num_t;
typedef bit<32> ipv4_addr_t;

const bit<16> TYPE_IPV4 = 0x800;
const bit<16> TYPE_IPV6 = 0x86dd;
const bit<16> TYPE_PWL = 0x88ab;
 



/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

header ethernet_t {
    bit<48> dstAddr;
    bit<48> srcAddr;
    bit<16> etherType;
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



struct headers{
    ethernet_t ethernet;
    ipv4_t   ipv4;
    ipv6_t   ipv6;
    powerlink_t pwl;
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
            TYPE_IPV4: parse_ipv4;
            TYPE_IPV6: parse_ipv6;
            TYPE_PWL: parse_pwl;
            default: accept;
        }
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
}




/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control IngressPipeImpl(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

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
