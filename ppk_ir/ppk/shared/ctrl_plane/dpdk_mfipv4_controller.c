#include "controller.h"
#include "messages.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_IPS 60000
extern int usleep(__useconds_t usec);
extern void notify_controller_initialized();
FILE *fp6;

controller c;

void fill_dest_guid_exact_table(uint32_t dest_na,uint8_t nxtHopMac[6],uint8_t port)
{
    char buffer[2048]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap,*ap2;
    struct p4_field_match_exact* exact;
    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "MyIngress.dest_na_exact_0");

    exact = add_p4_field_match_exact(te, 2048);
    strcpy(exact->header.name, "mf.dest_guid");
    memcpy(exact->bitmap, &dest_na, 4);
    exact->length = 4*8+0;
    a = add_p4_action(h, 2048);
    strcpy(a->description.name, "MyIngress.dest_na_forward");

    ap = add_p4_action_parameter(h,a,2048);
    strcpy(ap->name,"nxtHopMac");
    memcpy(ap->bitmap,nxtHopMac,6);
    ap->length = 6*8+0;

    ap2 = add_p4_action_parameter(h, a, 2048);
    strcpy(ap2->name, "port");
    memcpy(ap2->bitmap, &port, 1);
    // ap2->bitmap = port;
    // ap2->bitmap[1] = 0;
    ap2->length = 1*8+1;

    printf("NH-1\n");
    netconv_p4_header(h);
    netconv_p4_add_table_entry(te);
    netconv_p4_field_match_exact(exact);
    netconv_p4_action(a);
    netconv_p4_action_parameter(ap);
    netconv_p4_action_parameter(ap2);
    send_p4_msg(c, buffer, 2048);
    usleep(1200);
}

void fill_arp_exact_table(uint8_t target_ip[4],uint8_t gateway_ip[4], uint8_t gateway_mac[6])
{
    char buffer[2048];
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap;
    struct p4_action_parameter* ap2;
    struct p4_field_match_exact* exact;

    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "MyIngress.arp_exact_0");

    exact = add_p4_field_match_exact(te,2048);
    strcpy(exact->header.name,"arp.target_ip");
    memcpy(exact->bitmap,target_ip, 4);
    exact->length = 4*8+0;

    a = add_p4_action(h, 2048);
    strcpy(a->description.name, "MyIngress.arp_forward");

    ap = add_p4_action_parameter(h, a ,2048);
    strcpy(ap->name,"gateway_ip");
    memcpy(ap->bitmap, gateway_ip, 4);
    ap->length = 8*4+0;

    ap2 = add_p4_action_parameter(h, a ,2048);
    strcpy(ap2->name,"gateway_mac");
    memcpy(ap2->bitmap, gateway_mac, 6);
    ap2->length = 6*8+0;

    netconv_p4_header(h);
    netconv_p4_add_table_entry(te);
    netconv_p4_field_match_exact(exact);
    netconv_p4_action(a);
    netconv_p4_action_parameter(ap);
    netconv_p4_action_parameter(ap2);
    send_p4_msg(c, buffer, 2048);
    usleep(1200);
}

void fill_ipv4_exact_table(uint8_t srcaddr[4],uint8_t prefix,uint8_t dstaddr[4],uint8_t prefix2,uint32_t src_na,uint32_t dest_na,uint8_t port)
{
    char buffer[2048]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap,*ap1,*ap2;
    struct p4_field_match_lpm* lpm,*lpm1;
    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "MyIngress.ipv4_exact_0");

    lpm = add_p4_field_match_lpm(te, 2048);
    strcpy(lpm->header.name, "ipv4.dstAddr");
    memcpy(lpm->bitmap, srcaddr, 4);
    lpm->prefix_length = prefix;

    lpm1 = add_p4_field_match_lpm(te, 2048);
    strcpy(lpm1->header.name, "ipv4.dstAddr");
    memcpy(lpm1->bitmap, dstaddr, 4);
    lpm1->prefix_length = prefix2;

    a = add_p4_action(h, 2048);
    strcpy(a->description.name, "MyIngress.ipv4_mf_forward");

    ap = add_p4_action_parameter(h,a,2048);
    strcpy(ap->name,"src_na");
    memcpy(ap->bitmap,&src_na,4);
    ap->length = 4*8+0;

    ap1 = add_p4_action_parameter(h,a,2048);
    strcpy(ap1->name,"dst_na");
    memcpy(ap1->bitmap,&dest_na,4);
    ap1->length = 4*8+0;

    ap2 = add_p4_action_parameter(h, a, 2048);
    strcpy(ap2->name, "port");
    memcpy(ap2->bitmap, &port, 1);
    // ap2->bitmap = port;
    // ap2->bitmap[1] = 0;
    ap2->length = 1*8+1;

    printf("NH-1\n");
    netconv_p4_header(h);
    netconv_p4_add_table_entry(te);
    netconv_p4_field_match_lpm(lpm);
    netconv_p4_field_match_lpm(lpm1);
    netconv_p4_action(a);
    netconv_p4_action_parameter(ap);
    netconv_p4_action_parameter(ap1);
    netconv_p4_action_parameter(ap2);
    send_p4_msg(c, buffer, 2048);
    usleep(1200);
}


void dhf(void* b) {
       printf("Unknown digest received\n");
}


int read_config_from_file(char *filename)
{
	char line[100];
	uint8_t srcaddr[4];
    uint8_t dstaddr[4];
    // int n = -1;
    uint32_t dest_na;
    uint32_t src_na;
	uint8_t port;
    uint16_t ingress_port;
    uint8_t nxtHopMac[6];
    uint8_t gateway_ip[4];
    uint8_t target_ip[4];
    uint8_t gateway_mac[6];
    uint8_t prefix;
    uint8_t prefix2;
    // uint8_t dstmac[6];
	char dummy;
    FILE *fp6;
	FILE *f;
	f = fopen(filename,"r");
	if (f == NULL) return -1;


	int line_index = 0;
	while (fgets(line,sizeof(line),f)){

		line[strlen(line)-1] = '\0';
		line_index++;
		printf("Sor:%d.",line_index);
		if(line[0] == 'M'){
			if (9 == sscanf(line,"%c %x %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %hhd",
				&dummy,&dest_na,&nxtHopMac[0],&nxtHopMac[1],&nxtHopMac[2],&nxtHopMac[3],&nxtHopMac[4],&nxtHopMac[5],&port))
			{
				fill_dest_guid_exact_table(dest_na,nxtHopMac,port);
			}
			else{
                fclose(f);
                return -1;
			}
		}
        if(line[0] == 'I'){

			if (14 == sscanf(line,"%c %hhd.%hhd.%hhd.%hhd %hhd %hhd.%hhd.%hhd.%hhd %hhd %x %x %hhd",
				&dummy,&srcaddr[0],&srcaddr[1],&srcaddr[2],&srcaddr[3],&prefix,&dstaddr[0],&dstaddr[1],&dstaddr[2],&dstaddr[3],&prefix2,&src_na,&dest_na,&port))
			{
				fill_ipv4_exact_table(srcaddr,prefix,dstaddr,prefix2,src_na,dest_na,port);
			}
			else{
                fclose(f);
                return -1;
			}
		}
        if (line[0] == 'G')
        {
		    if (15 == sscanf(line,"%c %hhd.%hhd.%hhd.%hhd %hhd.%hhd.%hhd.%hhd %hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			&dummy,&target_ip[0],&target_ip[1],&target_ip[2],&target_ip[3],&gateway_ip[3],&gateway_ip[2],&gateway_ip[1],&gateway_ip[0],&gateway_mac[0],&gateway_mac[1],&gateway_mac[2],&gateway_mac[3],&gateway_mac[4],&gateway_mac[5]))
			{	
				fill_arp_exact_table(target_ip,gateway_ip,gateway_mac);
			}
            else
            {
                fclose(f);
                return -1;
			}
		}

	}
	fclose(f);
	return 0;
}


char* fn;
void init_complex(){

	if (read_config_from_file(fn)<0) {

    }
}


int main(int argc, char* argv[])
{
    // fp6 = fopen("/home/it-34/log/l3dst_ip_log.txt","a");
    //      if(fp6){
	//  	 fprintf(fp6,"diaoyong");
    //              fprintf(fp6,"\n");
    //              fclose(fp6);
    //          }
	if (argc>1) {
		if (argc!=2) {
			return -1;
		}
        fn = argv[1];
		c = create_controller_with_init(11111, 3, dhf, init_complex);
	}
    notify_controller_initialized();

	execute_controller(c);

	destroy_controller(c);

	return 0;
}
