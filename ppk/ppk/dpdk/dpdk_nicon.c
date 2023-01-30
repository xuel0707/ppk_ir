// Copyright 2018 Eotvos Lorand University, Budapest, Hungary
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/*****************************************************************************
Copyright: 2020-2022, xindawangyu Tech. Co., Ltd.
File name: dpdk_nicon.c
Description: the file contains a wrapper for the DPDK interface and function called by main.c.
Author: Ian
Version: 1.0
Date: 2021.12.31
*****************************************************************************/
#include <rte_ethdev.h>
#include "dpdk_nicon.h"
#include "des.h"

/* get the physical socket_id of the specified lcore*/
extern int get_socketid(unsigned lcore_id);

/* defined in dpdk_lib_init_hw.c*/
extern struct lcore_conf lcore_conf[RTE_MAX_LCORE];

/* init rx queue of core, init the mbuf_pool,enable promiscuous model for ports */
extern void dpdk_init_nic();

/* NOTICE: deprecated since DPDK 18.05*/
extern uint8_t get_nb_ports();

/* header_pool for normal packet forwarding,clone_pool for clone and copy model*/
struct rte_mempool *header_pool, *clone_pool;

/* soft encrypttion uses des algorithm,generating the key and padding*/
uint8_t *des_key;
uint16_t padding;

/* defined in des.c*/
extern void generate_key(uint8_t *key);

/* defined in dpdk_lib.c*/
extern struct rte_mempool *pktmbuf_pool[NB_SOCKETS];

/* Macro defined abcd consist of ipv4*/
#define MAKE_IPV4_ADDR(a, b, c, d) (a + (b << 8) + (c << 16) + (d << 24))

/* size of the key*/
#define DES_KEY_SIZE 8

/*************************************************
Function: send_burst
Description: Send burst of packets on an output interface
Calls: rte_eth_tx_burst/rte_pktmbuf_free
Called By: do_single_tx
Input: conf:description of lcore config,include lcore_hardware_conf and lcore_state.
       n:number of packets
       port:outport
Output: NULL
*************************************************/
static inline void send_burst(struct lcore_conf *conf, uint16_t n, uint8_t port)
{
    uint16_t queueid = conf->hw.tx_queue_id[port];
    struct rte_mbuf **m_table = (struct rte_mbuf **)conf->hw.tx_mbufs[port].m_table;

    int ret = rte_eth_tx_burst(port, queueid, m_table, n);
    if (unlikely(ret < n))
    {
        do
        {
            rte_pktmbuf_free(m_table[ret]);
        } while (++ret < n);
    }
}

/*************************************************
Function: tx_burst_queue_drain
Description: calculate diff_tsc value, when diff_tsc is less than the calculated value,
            a queue packet is sent
Calls: send_burst
Called By: main_loop_pre_rx
Input: lcdata:description of lcore data,include tsc、valid、lcore configure struct、mempool of dpdk、
       data and numbers of receive packet.
Output: NULL
*************************************************/
void tx_burst_queue_drain(struct lcore_data *lcdata)
{
    uint64_t cur_tsc = rte_rdtsc();

    uint64_t diff_tsc = cur_tsc - lcdata->prev_tsc;
    if (unlikely(diff_tsc > lcdata->drain_tsc))
    {
        for (unsigned portid = 0; portid < get_nb_ports(); portid++)
        {
            if (lcdata->conf->hw.tx_mbufs[portid].len == 0)
                continue;

            send_burst(lcdata->conf,
                       lcdata->conf->hw.tx_mbufs[portid].len,
                       (uint8_t)portid);
            lcdata->conf->hw.tx_mbufs[portid].len = 0;
        }

        lcdata->prev_tsc = cur_tsc;
    }
}

/*************************************************
Function: add_packet_to_queue
Description: put a packet into rx queue
Calls: NULL
Called By: send_packetout_from_controller/dpdk_send_packet
Input: mbuf:description of packet.
       port:physical port
       lcore_id:logical core 
Output: queue length
*************************************************/
static uint16_t
add_packet_to_queue(struct rte_mbuf *mbuf, uint8_t port, uint32_t lcore_id)
{
    struct lcore_conf *conf = &lcore_conf[lcore_id];
    uint16_t queue_length = conf->hw.tx_mbufs[port].len;
    conf->hw.tx_mbufs[port].m_table[queue_length] = mbuf;
    conf->hw.tx_mbufs[port].len++;
    return conf->hw.tx_mbufs[port].len;
}

/*************************************************
Function: mcast_out_pkt
Description: creating replicas of a packet for mcast
Calls: rte_pktmbuf_alloc/rte_pktmbuf_clone
Called By: NULL
Input: pkt:description of packet.
       use_clone:0 show execution clone
Output: rte_mbuf:cloned packet
*************************************************/
static inline struct rte_mbuf *
mcast_out_pkt(struct rte_mbuf *pkt, int use_clone)
{
    struct rte_mbuf *hdr;

    debug("mcast_out_pkt new mbuf is needed...\n");
    /* Create new mbuf for the header. */
    if ((hdr = rte_pktmbuf_alloc(header_pool)) == NULL)
        return (NULL);

    debug("hdr is allocated\n");

    /* If requested, then make a new clone packet. */
    if (use_clone != 0 &&
        (pkt = rte_pktmbuf_clone(pkt, clone_pool)) == NULL)
    {
        rte_pktmbuf_free(hdr);
        return (NULL);
    }

    debug("setup ne header\n");

    /* prepend new header */
    hdr->next = pkt;

    /* update header's fields */
    hdr->pkt_len = (uint16_t)(hdr->data_len + pkt->pkt_len);
    hdr->nb_segs = (uint8_t)(pkt->nb_segs + 1);

    /* copy metadata from source packet*/
    hdr->port = pkt->port;
    hdr->vlan_tci = pkt->vlan_tci;
    hdr->vlan_tci_outer = pkt->vlan_tci_outer;
    hdr->tx_offload = pkt->tx_offload;
    hdr->hash = pkt->hash;

    hdr->ol_flags = pkt->ol_flags;

    __rte_mbuf_sanity_check(hdr, 1);
    return (hdr);
}

/*************************************************
Function: send_packetout_from_controller
Description: controller send packet-out to dataplane
Calls: add_packet_to_queue/rte_pktmbuf_append
Called By: ctrl_packet_out
Input: ctrl_m:defined in handlers.h,describing struct interaction with controller.
Output: NULL
*************************************************/
void send_packetout_from_controller(struct p4_ctrl_msg *ctrl_m)
{
    // debug("Dummy callback - payload:%s\n\n\n", ctrl_m->packet);
    // debug("Dummy callback - len:%d\n\n\n", ctrl_m->len);
    #if PPK_VLAN
        uint32_t lcore_id = 1;
        struct rte_mbuf *buff;
        // struct vlan_tag packetout_vlan;
        packetout_vlan.pri = 0;
        packetout_vlan.cfi = 0;
        buff = rte_pktmbuf_alloc(header_pool);
        // buff->pkt_len = (ctrl_m->len) + 4;
        // buff->data_len = (ctrl_m->len) + 4;
        uint8_t *data = (uint8_t *)rte_pktmbuf_append(buff, ctrl_m->len);
        memcpy(data, ctrl_m->packet, ctrl_m->len);
        int egress_port1 = (int)ctrl_m->metadata[0][1];
        packetout_vlan.vid = (0xfff & egress_port1) + (COME_ID*100);
        packetout_vlan.type = 0x8100;
        uint32_t vlan1 = rte_cpu_to_be_32(*(uint32_t *)&packetout_vlan);
        uint8_t* head = (uint8_t*)rte_pktmbuf_prepend(buff,4);
        memcpy(head,data,12);
        memcpy(head + 12, &vlan1, 4);
        int egress_port = egress_port1 % 2;
        debug("Dummy callback - egress_port:%d\n\n\n", egress_port);
    #else
        uint32_t lcore_id = 1;
        struct rte_mbuf *buff;
        // buff->pkt_len = ctrl_m->len;
        
        // buff->data_len = ctrl_m->len;
        buff = rte_pktmbuf_alloc(header_pool);
        uint8_t *data = (uint8_t *)rte_pktmbuf_append(buff, ctrl_m->len);
        memcpy(data, ctrl_m->packet, ctrl_m->len);
        int egress_port = (int)ctrl_m->metadata[0][1];
        //int egress_port = strtol(ctrl_m->metadata[0], NULL, 10);
        debug("Dummy callback - egress_port:%d\n", egress_port);
        //debug("Dummy callback - ctrl_m->packet:%s\n\n\n", ctrl_m->packet[]);
    #endif
    uint16_t queue_length = add_packet_to_queue(buff, egress_port, lcore_id);
    void rte_pktmbuf_free(buff);
}


/*************************************************
Function: dpdk_send_packet
Description: Encapsulate send_burst interface
Calls: add_packet_to_queue/send_burst
Called By: send_single_packet
Input: mbuf:description of packet
       port:physical port
       lcore_id:logical core
Output: NULL
*************************************************/
static void dpdk_send_packet(struct rte_mbuf *mbuf, uint8_t port, uint32_t lcore_id)
{
    struct lcore_conf *conf = &lcore_conf[lcore_id];
    uint16_t queue_length = add_packet_to_queue(mbuf, port, lcore_id);

    if (unlikely(queue_length == MAX_PKT_BURST))
    {
        debug("    :: BURST SENDING DPDK PACKETS - port:%d\n", port);
        send_burst(conf, MAX_PKT_BURST, port);
        queue_length = 0;
    }

    conf->hw.tx_mbufs[port].len = queue_length;
}

/*************************************************
Function: send_single_packet
Description: Enqueue a single packet, and send burst if queue is filled
Calls: dpdk_send_packet/send_burst
Called By: broadcast_packet/bitmcast_packet/send_packet
Input: lcdata:description of lcore data,include tsc、valid、lcore configure struct、mempool of dpdk、
       data and numbers of receive packet.
       pd:description of packet,include mbuf of dpdk、packet data and packet header .etc
       pkt:mbuf struct
       egress_port:outport
       ingress_port:inport
       send_clone:clone sign
Output: NULL
*************************************************/
void send_single_packet(struct lcore_data *lcdata, packet_descriptor_t *pd, packet *pkt, int egress_port, int ingress_port, bool send_clone)
{
    uint32_t lcore_id = rte_lcore_id();
    struct rte_mbuf *mbuf = (struct rte_mbuf *)pkt;
    dbg_bytes(rte_pktmbuf_mtod(mbuf, uint8_t *), rte_pktmbuf_pkt_len(mbuf), "   " T4LIT(<<, outgoing) " " T4LIT(Emitting, outgoing) " packet on port " T4LIT(% d, port) " (" T4LIT(% d) " bytes): ", egress_port, rte_pktmbuf_pkt_len(mbuf));
#if PPK_VLAN
    uint8_t *data = rte_pktmbuf_mtod(mbuf, void *);
    vlan.pri = 0;
    vlan.cfi = 0;
    debug("       v1model :value of egress port field  " T4LIT(% 08x) "\n", egress_port);
    vlan.vid = (COME_ID*100) + (GET_INT32_AUTO_PACKET(pd, header_instance_all_metadatas, field_standard_metadata_t_ingress_port) * 64);
    uint16_t vlantype32 = (uint16_t)GET_INT32_AUTO_PACKET(pd, header_instance_all_metadatas, field_standard_metadata_t_instance_type);
    debug("       v1model :value of ethernet-type field  " T4LIT(% 08x) "\n", vlantype32);
    vlan.type = rte_cpu_to_be_16(vlantype32);
    uint32_t vlan_of = *(uint32_t *)&vlan;
    uint32_t vlan1 = rte_cpu_to_be_32(vlan_of);
    memcpy(data + 14, &vlan1, sizeof(vlan));
    int fix_egress_port = egress_port % 2;
    dbg_bytes(rte_pktmbuf_mtod(mbuf, uint8_t *), rte_pktmbuf_pkt_len(mbuf), "   " T4LIT(>>>>, outgoing) " " T4LIT(Emitting, outgoing) " packet on port " T4LIT(% d, port) " (" T4LIT(% d) " bytes): ", fix_egress_port, rte_pktmbuf_pkt_len(mbuf));
    dpdk_send_packet(mbuf, fix_egress_port, lcore_id);
#else
    // rte_pktmbuf_dump(stdout, mbuf, rte_pktmbuf_pkt_len(mbuf));
    dpdk_send_packet(mbuf, egress_port, lcore_id);
#endif
}

/*************************************************
Function: init_queues
Description: print log base on dbg model
Calls: NULL
Called By: init_lcore_data
Input: lcdata:description of lcore data,include tsc、valid、lcore configure struct、mempool of dpdk、
       data and numbers of receive packet.
Output: NULL
*************************************************/
void init_queues(struct lcore_data *lcdata)
{
    for (unsigned i = 0; i < lcdata->conf->hw.n_rx_queue; i++)
    {
        unsigned portid = lcdata->conf->hw.rx_queue_list[i].port_id;
        uint8_t queueid = lcdata->conf->hw.rx_queue_list[i].queue_id;
        RTE_LOG(INFO, P4_FWD, " -- lcoreid=%u portid=%u rxqueueid=%hhu\n", rte_lcore_id(), portid, queueid);
    }
}

/*************************************************
Function: init_lcore_data
Description: initialize part data of lcdata struct
Calls: init_queues/get_socketid
Called By: dpdk_main_loop
Input: NULL
Output: struct lcore_data
*************************************************/
struct lcore_data init_lcore_data()
{
    struct lcore_data lcdata = {
        .drain_tsc = (rte_get_tsc_hz() + US_PER_S - 1) / US_PER_S * BURST_TX_DRAIN_US,
        .prev_tsc = 0,

        .conf = &lcore_conf[rte_lcore_id()],
        .mempool = pktmbuf_pool[get_socketid(rte_lcore_id())], // TODO: Check for MULTI-SOCKET CASE !!!!

        .is_valid = lcdata.conf->hw.n_rx_queue != 0,
    };

    if (lcdata.is_valid)
    {
        RTE_LOG(INFO, P4_FWD, "entering main loop on lcore %u\n", rte_lcore_id());

        init_queues(&lcdata);
    }
    else
    {
        RTE_LOG(INFO, P4_FWD, "lcore %u has nothing to do\n", rte_lcore_id());
    }

    return lcdata;
}


/* TODO add fault tolerance*/
bool core_is_working(struct lcore_data *lcdata)
{
    return true;
}

/* TODO add fault tolerance*/
bool is_packet_handled(packet_descriptor_t *pd, struct lcore_data *lcdata)
{
    return true;
}

/*************************************************
Function: receive_packet
Description: mbuf memory space
Calls: rte_prefetch0/rte_pktmbuf_mtod
Called By: do_single_rx
Input: pd:description of packet,include mbuf of dpdk、packet data and packet header .etc
       lcdata:description of lcore data,include tsc、valid、lcore configure struct、mempool of dpdk、
       data and numbers of receive packet.
       pkt_idx:order of packet
Output: bool 
*************************************************/
bool receive_packet(packet_descriptor_t *pd, struct lcore_data *lcdata, unsigned pkt_idx)
{
    packet *p = lcdata->pkts_burst[pkt_idx];
    rte_prefetch0(rte_pktmbuf_mtod(p, void *));
    pd->data = rte_pktmbuf_mtod(p, uint8_t *);
    pd->wrapper = p;
    return true;
}

/*************************************************
Function: free_packet
Description: free mbuf
Calls: rte_pktmbuf_free
Called By: bitmcast_packet/do_single_tx/
Input: pd:description of packet,include mbuf of dpdk、packet data and packet header .etc
Output: NULL
*************************************************/
void free_packet(packet_descriptor_t *pd)
{
    rte_pktmbuf_free(pd->wrapper);
}

/*************************************************
Function: init_storage
Description: init dpdk mempool
Calls: rte_pktmbuf_pool_create
Called By: main
Input: NULL
Output: NULL
*************************************************/
void init_storage()
{
    /* Needed for L2 multicasting - e.g. acting as a hub
        cloning headers and sometimes packet data*/
    header_pool = rte_pktmbuf_pool_create("header_pool", NB_HDR_MBUF, 32,
                                          0, HDR_MBUF_DATA_SIZE, rte_socket_id());

    if (header_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init header mbuf pool\n");

    clone_pool = rte_pktmbuf_pool_create("clone_pool", NB_CLONE_MBUF, 32,
                                         0, 0, rte_socket_id());

    if (clone_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init clone mbuf pool\n");
}

/*************************************************
Function: init_encypt_key
Description: generate encrypt key and store file,then decryption can use the file
Calls: generate_key
Called By: main
Input: NULL
Output: NULL
*************************************************/
void init_encypt_key()
{
    static FILE *key_file;
    key_file = fopen("./key.txt", "wb");
    if (!key_file)
    {
        rte_exit(EXIT_FAILURE, "Could not open file to write key.\n");
    }
    // unsigned int iseed = (unsigned int)time(NULL);
    // srand (iseed);
    des_key = (uint8_t *)malloc(8 * sizeof(uint8_t));
    generate_key(des_key);
    short int bytes_written;
    bytes_written = fwrite(des_key, 1, DES_KEY_SIZE, key_file);
    if (bytes_written != DES_KEY_SIZE)
    {
        fclose(key_file);
        free(des_key);
        rte_exit(EXIT_FAILURE, "Error writing key to output file.\n");
    }
    fclose(key_file);
}

/*************************************************
Function: main_loop_pre_rx
Description: generate encrypt key and store file,then decryption can use the file
Calls: tx_burst_queue_drain
Called By: dpdk_main_loop
Input: lcdata:description of lcore data,include tsc、valid、lcore configure struct、mempool of dpdk、
       data and numbers of receive packet.
Output: NULL
*************************************************/
void main_loop_pre_rx(struct lcore_data *lcdata)
{
    tx_burst_queue_drain(lcdata);
}

/* TODO handle after receive packets */
void main_loop_post_rx(struct lcore_data *lcdata)
{
}

/* TODO handle after receive one packet*/
void main_loop_post_single_rx(struct lcore_data *lcdata, bool got_packet)
{
}

/* port corresponding to the queue*/
uint32_t get_portid(struct lcore_data *lcdata, unsigned queue_idx)
{
    return lcdata->conf->hw.rx_queue_list[queue_idx].port_id;
}

/* handle KNI interface*/
void main_loop_rx_group(struct lcore_data *lcdata, unsigned queue_idx)
{
    uint8_t queue_id = lcdata->conf->hw.rx_queue_list[queue_idx].queue_id;
#if KNI_STORAGE
    unsigned num;
    unsigned nb_tx = 0;
    uint8_t kni_lcore_id = rte_lcore_id();
    if (kni_lcore_id == 0)
    {
        uint8_t kni_queue_id = kni_lcore_id;
        struct rte_mbuf *kni_pkts_burst[MAX_PKT_BURST];
        // printf("num：%d lcore_id:%d\n",num,lcore_id);
        lcdata->nb_rx = rte_eth_rx_burst((uint8_t)get_portid(lcdata, queue_idx), queue_id, lcdata->pkts_burst, MAX_PKT_BURST);
        num = my_kni_egress(kni_pkts_burst, 3);
        for (unsigned i = 0; i < num; i++)
        {
            lcdata->pkts_burst[lcdata->nb_rx] = kni_pkts_burst[i];
            lcdata->nb_rx++;
            printf("lcdata-nb_rx:%d\n", lcdata->nb_rx);
        }
        // nb_tx = rte_eth_tx_burst(0, kni_queue_id, kni_pkts_burst, (uint16_t)num);
        my_kni_burst_free_mbufs(&kni_pkts_burst[0], num);
    }
    else
    {
        lcdata->nb_rx = rte_eth_rx_burst((uint8_t)get_portid(lcdata, queue_idx), queue_id, lcdata->pkts_burst, MAX_PKT_BURST);
    }
#else
    lcdata->nb_rx = rte_eth_rx_burst((uint8_t)get_portid(lcdata, queue_idx), queue_id, lcdata->pkts_burst, MAX_PKT_BURST);
#endif
}

/*************************************************
Function: get_pkt_count_in_group
Description: return number of packets
Calls: NULL
Called By: do_rx
Input: lcdata:description of lcore data,include tsc、valid、lcore configure struct、mempool of dpdk、
       data and numbers of receive packet.
Output: nb_rx
*************************************************/
unsigned get_pkt_count_in_group(struct lcore_data *lcdata)
{
    return lcdata->nb_rx;
}

/*************************************************
Function: get_queue_count
Description: return number of queue
Calls: NULL
Called By: do_rx
Input: lcdata:description of lcore data,include tsc、valid、lcore configure struct、mempool of dpdk、
       data and numbers of receive packet.
Output: n_rx_queue
*************************************************/
unsigned get_queue_count(struct lcore_data *lcdata)
{
    return lcdata->conf->hw.n_rx_queue;
}

/*************************************************
Function: initialize_nic
Description: called by dpdk_lib_init_hw.c
Calls: dpdk_init_nic
Called By: main
Input: NULL
Output: NULL
*************************************************/
void initialize_nic()
{
    dpdk_init_nic();
}

/* DPDK default model*/
int launch_count()
{
    return 1;
}

/* print abnormal log*/
void ppk_abnormal_exit(int retval, int idx)
{
    debug(T4LIT(Abnormal exit, error) ", code " T4LIT(% d) ".\n", retval);
}

/* print normal log*/
int ppk_normal_exit()
{
    debug(T4LIT(Normal exit., success) "\n");
    return 0;
}

/* TODO lcore config before run ppk */
void ppk_pre_launch(int idx)
{
}

/* TODO lcore handke after exit ppk */
void ppk_post_launch(int idx)
{
}

/* enable which port is working*/
extern uint32_t enabled_port_mask;
uint32_t get_port_mask()
{
    return enabled_port_mask;
}

/* get numbers of available ports*/
extern uint8_t get_nb_ports();
uint8_t get_port_count()
{
    return get_nb_ports();
}
