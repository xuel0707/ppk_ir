/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

typedef bit<9> port_t;
typedef bit<48> mac_t;
const bit<16> TYPE_IPV4 = 0x0800;
const bit<16> TYPE_ARP = 0x0806;
const bit<16> TYPE_MF = 0x27C0;
const bit<16> TYPE_GEO = 0x8947;
const bit<9> PORT_ONOS =255;
const port_t CPU_PORT = 255;

/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

header ethernet_t {
    bit<48> dstAddr;
    bit<48> srcAddr;
    bit<16> ether_type;
}

header ipv4_t {
    bit<4>  version;
    bit<4>  ihl;
    bit<6>  dscp;
    bit<2>  ecn;
    bit<16> totalLen;
    bit<16> identification;
    bit<3>  flags;
    bit<13> fragOffset;
    bit<8>  ttl;
    bit<8>  protocol;
    bit<16> hdrChecksum;
    bit<32> srcAddr;
    bit<32> dstAddr;
}
header arp_t {
    bit<16> hardware_type;
    bit<16> protocol_type;
    bit<8>  HLEN;
    bit<8>  PLEN;
    bit<16> OPER;
    bit<48> sender_ha;
    bit<32> sender_ip;
    bit<48> target_ha;
    bit<32> target_ip;
}

header mf_guid_t{
    bit<32> mf_type;
	bit<32> src_guid;
    bit<32> dest_guid;
    bit<32> src_na;
    bit<32> dst_na;
    bit<32> paload_size;
    bit<32> seq_num;
    bit<32> hop_id;
}

header geo_t{
    bit<32>  basic;
    bit<32>  common_a;
    bit<32>  common_b;
    bit<32>  src_geoid;
    bit<32>  dst_geoid;

}
// Packet-in header. Prepended to packets sent to the controller and used to
// carry the original ingress port where the packet was received.
@controller_header("packet_in")
header packet_in_header_t {
    bit<7> _padding;
    bit<9> ingress_port;
}

// Packet-out header. Prepended to packets received by the controller and used
// to tell the switch on which port this packet should be forwarded.
@controller_header("packet_out")
header packet_out_header_t {
    bit<7> _padding;
    bit<9> egress_port;
}

struct headers{
    ethernet_t          ethernet;
    mf_guid_t           mf;
    geo_t               geo;
    packet_out_header_t packet_out;
    packet_in_header_t  packet_in;
    ipv4_t              ipv4;
    arp_t               arp;
}

struct metadata {
    /* empty */
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    @name(".start") state start {
        transition select(standard_metadata.ingress_port) {
            CPU_PORT: parse_packet_out;
            default: parse_ethernet;
        }    
    }

    @name(".parse_packet_out") state parse_packet_out {
        packet.extract(hdr.packet_out);
        transition parse_ethernet;
    }

    @name(".parse_ethernet") state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.ether_type) { 
            TYPE_IPV4 : parse_ipv4;
            TYPE_MF: parse_mf;
            TYPE_GEO: parse_geo;
            TYPE_ARP: parse_arp;
            default: accept;
        }
    }

    @name(".parse_ipv4") state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition accept;
    }
    @name(".parse_mf") state parse_mf{
        packet.extract(hdr.mf);
		transition accept;
    }
    @name(".parser_arp") state parse_arp{
        packet.extract(hdr.arp);
        transition accept;
    }
    @name(".parser_geo") state parse_geo{
        packet.extract(hdr.geo);
        transition accept;
    }
}




/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action send_to_cpu() {
        standard_metadata.egress_spec = CPU_PORT;
        hdr.packet_in.setValid();
        hdr.packet_in.ingress_port = standard_metadata.ingress_port;
    }

    action set_out_port(port_t port) {
        // Specifies the output port for this packet by setting the
        // corresponding metadata.
        standard_metadata.egress_spec = port;
    }

    action mf_ipv4_forward(mac_t nxtHopMac, port_t port) {
		hdr.mf.setInvalid();
        hdr.ethernet.ether_type = TYPE_IPV4;
        hdr.ethernet.dstAddr = nxtHopMac;
        standard_metadata.egress_spec = port;
    }
    action geo_ipv4_forward(mac_t nxtHopMac, port_t port) {
		hdr.geo.setInvalid();
        hdr.ethernet.ether_type = TYPE_IPV4;
        hdr.ethernet.dstAddr = nxtHopMac;
        standard_metadata.egress_spec = port;
    }

    action dest_guid_forward(port_t port) {
        standard_metadata.egress_spec = port;
    }

    action dest_geoid_forward(port_t port) {
        standard_metadata.egress_spec = port;
    }
    action drop() {
        mark_to_drop(standard_metadata);
    }

    action ipv4_mf_forward(bit<32> src_guid,bit<32> dst_guid, port_t port) {
        hdr.ethernet.ether_type = TYPE_MF;
        hdr.mf.setValid();
        hdr.mf.mf_type = 0x0;
	    hdr.mf.src_guid = src_guid;
        hdr.mf.dest_guid = dst_guid;
        hdr.mf.src_na = 0x10;
        hdr.mf.dst_na = 0x11;
        hdr.mf.paload_size = standard_metadata.packet_length - 56;
        hdr.mf.seq_num = 0x0;
        hdr.mf.hop_id = 0x0;
        standard_metadata.egress_spec = port;
    }

    action ipv4_geo_forward(bit<32> src_geoid,bit<32> dst_geoid, port_t port) {
        hdr.ethernet.ether_type = TYPE_GEO;
        hdr.geo.setValid();
        hdr.geo.basic = 0x0100f10a;
	    hdr.geo.common_a = 0x20410080;
        hdr.geo.common_b = 0x00330a00;
        hdr.geo.src_geoid = src_geoid;
        hdr.geo.dst_geoid = dst_geoid;
        standard_metadata.egress_spec = port;
    }

    action ipv4_forward(port_t port) {
        standard_metadata.egress_spec = port;
    }

    action arp_forward(bit<32>gateway_ip, bit<48>gateway_mac){
        standard_metadata.egress_spec=standard_metadata.ingress_port;
        hdr.ethernet.dstAddr=hdr.ethernet.srcAddr;
        hdr.ethernet.srcAddr=gateway_mac;
        hdr.arp.target_ha=hdr.arp.sender_ha;
        hdr.arp.target_ip=hdr.arp.sender_ip;
        hdr.arp.sender_ip=gateway_ip;
        hdr.arp.sender_ha=gateway_mac;
        hdr.arp.OPER=0x2;
    }
	
    table dest_guid_exact {
        actions = {
            dest_guid_forward;
            mf_ipv4_forward;
            send_to_cpu;
        }
        key = {
            hdr.mf.dest_guid: exact;
        }
        size = 1024;
        default_action = send_to_cpu();
    }

	table ipv4_exact {
        actions = {
            ipv4_forward;
            ipv4_mf_forward;
            ipv4_geo_forward;
            send_to_cpu;
        }
        key = {
            hdr.ipv4.srcAddr: ternary;
            hdr.ipv4.dstAddr: ternary;
        }
        size = 1024;
        default_action = send_to_cpu();
    }
    table dest_geoid_exact {
        actions = {
            dest_geoid_forward;
            geo_ipv4_forward;
            send_to_cpu;
        }
        key = {
            hdr.geo.dst_geoid: exact;
        }
        size = 1024;
        default_action = send_to_cpu();
    }
	table arp_exact{
        key={
            hdr.arp.target_ip :exact;
        }
        actions={
            arp_forward;
            send_to_cpu;
        }
    }
    table l2_fwd {
        key = {
            standard_metadata.ingress_port  : ternary;
            hdr.ethernet.dstAddr            : ternary;
            hdr.ethernet.srcAddr            : ternary;
            hdr.ethernet.ether_type         : ternary;
        }
        actions = {
            set_out_port;
            send_to_cpu;
            drop;
            NoAction;
        }
        default_action = NoAction();
    }
    apply {
	        if (standard_metadata.ingress_port == CPU_PORT) {
                standard_metadata.egress_port = hdr.packet_out.egress_port;
				hdr.packet_out.setInvalid();
			} else {
                if(hdr.arp.isValid()){  
                    arp_exact.apply();  
                }
                else if (l2_fwd.apply().hit){
                    return;
                }
                else if(hdr.mf.isValid()) {
                    dest_guid_exact.apply();
                }
                else if(hdr.geo.isValid()) {
                    dest_geoid_exact.apply();
                }
                else if(hdr.ipv4.isValid()){
                    ipv4_exact.apply();
                }
                else{
                    drop();
                }
			}
		}
}

control egress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    apply {
    }
}



/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {

        apply{
            packet.emit(hdr.packet_in);
            packet.emit(hdr.ethernet);
			packet.emit(hdr.mf);
            packet.emit(hdr.geo);
            packet.emit(hdr.arp);
            packet.emit(hdr.ipv4);
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
	MyParser(),
	verifyChecksum(),
	MyIngress(),
	egress(),
	computeChecksum(),
	MyDeparser()
) main;
