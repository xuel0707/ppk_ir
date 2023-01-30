/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>
typedef bit<9> port_t;
typedef bit<48> mac_t;
const bit<16> TYPE_GEO = 0x8947;

const bit<4> TYPE_geo_beacon = 0x1;
   
const bit<4> TYPE_geo_gbc = 0x4;     

const bit<4> TYPE_geo_beacon_s = 0xA;
   
const bit<4> TYPE_geo_gbc_s = 0xB;  

const bit<4> TYPE_geo_tsb= 0xC; 

const bit<16> PROTOCOL_ESP = 0XCC;

const bit<9> PORT_ONOS =255;
const bit<9> PORT_BIT_MCAST =254;

/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

header ethernet_t {
    bit<48>   dstAddr;
    bit<48>   srcAddr;
    bit<16>   ether_type;
}

header ipv4_t {
    bit<4>    version;
    bit<4>    ihl;
    bit<8>    diffserv;
    bit<16>   totalLen;
    bit<16>   identification;
    bit<3>    flags;
    bit<13>   fragOffset;
    bit<8>    ttl;
    bit<8>    protocol;
    bit<16>   hdrChecksum;
    bit<32>   srcAddr;
    bit<32>   dstAddr;
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

header beacon_t{
    bit<64> gnaddr;
    bit<32> tst;
    bit<32> lat;
    bit<32> longg;
    bit<1> pai;
    bit<15> s;
    bit<16> h;
    //是否可以在header中使用结构体
}

header mf_guid_t{
    bit<32> mf_type;
	bit<32> src_guid;
    bit<32> dest_guid;
}

header esp_t {
	bit<32> spi;
	bit<32> sequenceNumber;
}

struct metadata {
    /* empty */
}

struct headers {
    ethernet_t  ethernet;
    ipv4_t  ipv4;
    geo_t  geo;
    gbc_t  gbc;
    beacon_t  beacon;
    mf_guid_t mf;
    esp_t esp;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    @name(".start") state start {
        transition parse_ethernet;
    }

    @name(".parse_ethernet") state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.ether_type) {
            16w0x0800 : parse_ipv4;
            16w0x8947 : parse_geo;
            16w0x27c0 : parse_mf;
            default: accept;
        }
    }

    @name(".parse_geo") state parse_geo{
        packet.extract(hdr.geo);
        transition select(hdr.geo.ht) { //要根据ht的大小来判断选取的字段
            TYPE_geo_beacon: parse_beacon; //0x01
            TYPE_geo_gbc: parse_gbc;       //0x04
            TYPE_geo_beacon_s: parse_beacon_s; //0x0A
            TYPE_geo_gbc_s: parse_gbc_s;       //0x0B
            default: accept;
        }
    }

    @name(".parse_mf") state parse_mf{
        packet.extract(hdr.mf);
		transition select(hdr.mf.mf_type) { //要根据ht的大小来判断选取的字段
            0x0A: parse_esp; //0x0A
            default: accept;
        }
    }

    @name(".parse_ipv4") state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition accept;
    }

    @name(".parse_beacon") state parse_beacon{
        packet.extract(hdr.beacon);
        transition accept;
    }

    @name(".parse_gbc") state parse_gbc{
        packet.extract(hdr.gbc);
        transition accept;
    }


    @name(".parse_beacon_s") state parse_beacon_s{
        packet.extract(hdr.beacon);
        transition parse_esp;
    }

    @name(".parse_gbc_s") state parse_gbc_s{
        packet.extract(hdr.gbc);
        transition parse_esp;
    }


    @name(".parse_esp") state parse_esp {
		packet.extract(hdr.esp);
		transition accept;
	}

}

/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout headers hdr, inout metadata meta) {   
    apply {  }
}


/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/
extern void decrypt_with_payload<Q>(in Q first_head_offset, in Q last_head_length, in Q last_head_offset);
control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {
    action unicast(bit<9> port) {
        standard_metadata.egress_spec = port;
    }

    action fwd2ONOS() {
       standard_metadata.egress_spec = PORT_ONOS; //clone3(CloneType.I2E, CPU_CLONE_SESSION_ID, standard_metadata);
    }

    action drop() {
        mark_to_drop(standard_metadata);
    }


    action multicast(bit <32> bitcast) {

        standard_metadata.egress_spec = PORT_BIT_MCAST;
        standard_metadata.bit_mcast = bitcast;
    }//用于组播的动作函数

    action geo_decry (bit<8> name1, bit<8>name2, bit<8>name3) {
        //本acntion添加对数据包的解密操作和recirulate动作，包括以下三个部分；
        //（1）对于GEOsec的解密操作，GEOSec中的GEO包头要被去掉，并同步在pd headers中去除包头；
        // (2) 将ethertype字段根据原GEOSec包头中的协议号更改；
        // (3) recirculate,将数据发送至parser重新操作；
        decrypt_with_payload(name1, name2,name3);
        standard_metadata.recirculate_flag = 1;
    }

    table geo_spd {
        key = {
            hdr.gbc.geoAreaPosLat: exact;
            hdr.gbc.geoAreaPosLon: exact;
        }
        actions = {
            geo_decry;
            NoAction;
        }
        default_action= NoAction;
    }



    table eth_exact {
        key = {
            standard_metadata.ingress_port: ternary;
            hdr.ethernet.dstAddr: ternary;
            hdr.ethernet.srcAddr: ternary;
            hdr.ethernet.etherType: ternary;
        }
        actions = {
            fwd2ONOS;
            drop;
        }
        default_action = drop();
    }

    table gbc_exact {
        actions = {
            multicast;
            unicast;
            fwd2ONOS;
        }
        key = {
            hdr.gbc.geoAreaPosLat: exact;
            hdr.gbc.geoAreaPosLon: exact;
            hdr.gbc.disa: exact;
            hdr.gbc.disb: exact;
        }
        size = 1024;
        default_action = fwd2ONOS();
    }

    apply {
        if (hdr.geo.isValid()) {
            if(hdr.geo.nh_common == 0XB) {
                geo_spd.apply();
            }
            else{
                gbc_exact.apply();
            }
        }
        else{
            eth_exact.apply();
        }
        
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/
control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {

    apply {  
    }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout headers  hdr, inout metadata meta) {
    apply {
        /**update_checksum(
            hdr.ipv4.isValid(),
            { hdr.ipv4.version,
              hdr.ipv4.ihl,
              hdr.ipv4.diffserv,
              hdr.ipv4.totalLen,
              hdr.ipv4.identification,
              hdr.ipv4.flags,
              hdr.ipv4.fragOffset,
              hdr.ipv4.ttl,
              hdr.ipv4.protocol,
              hdr.ipv4.srcAddr,
              hdr.ipv4.dstAddr },
           hdr.ipv4.hdrChecksum,
           HashAlgorithm.csum16);**/
            //decrypt_with_payload();
    }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.geo);

    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;

