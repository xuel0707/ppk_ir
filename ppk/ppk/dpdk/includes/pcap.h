#ifndef pcap_h
#define pcap_h

typedef struct pcap_file_header {
    uint32_t magic;
    uint16_t major;
    uint16_t minor;
    int thiszone;
    int sigfigs;
    int snaplen;
    int linktype;
}pcap_file_header;

typedef struct timestamp_t{
    uint32_t timestamp_s;
    uint32_t timestamp_ms;
}timestamp_t;

typedef struct pcap_header{
    timestamp_t ts;
    uint32_t caplen;
    uint32_t len;
}pcap_header;

// void encode_pcap(struct packet_descriptor_t* pd,FILE* f,const char* fname);

#endif
