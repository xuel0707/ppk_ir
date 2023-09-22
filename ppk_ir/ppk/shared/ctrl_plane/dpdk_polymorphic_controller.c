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

controller c;

void fill_geo_exact_table(uint16_t isvalid[6],uint8_t flag,uint8_t port,uint32_t bitcast){
    char buffer[2048]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a,* a1, *a2;
    struct p4_action_parameter* ap,* ap1;
    struct p4_field_match_exact* exact,*exact1,*exact2,*exact3;

    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "IngressImpl.geo_exact_0");

    exact = add_p4_field_match_exact(te,2048);
    strcpy(exact->header.name,"hdr.gbc.geoAreaPosLat");
    memcpy(exact->bitmap,&isvalid[0],4);

    exact1 = add_p4_field_match_exact(te,2048);
    strcpy(exact1->header.name,"hdr.gbc.geoAreaPosLon");
    memcpy(exact1->bitmap,&isvalid[2],4);

    exact2 = add_p4_field_match_exact(te,2048);
    strcpy(exact2->header.name,"hdr.gbc.disa");
    memcpy(exact2->bitmap,&isvalid[4],2);
    exact2->length = 16*1+0;

    exact3 = add_p4_field_match_exact(te,2048);
    strcpy(exact3->header.name,"hdr.gbc.disb");
    memcpy(exact3->bitmap,&isvalid[5],2);
    exact3->length = 16*1+0;

    if(flag == 1){
        a1 = add_p4_action(h,2048);
        strcpy(a1->description.name,"IngressImpl.geo_set_egress_port");
        ap1 = add_p4_action_parameter(h, a1, 2048);
        strcpy(ap1->name, "port");
        memcpy(ap1->bitmap, &port, 1);
        // // ap1->bitmap = port;
        // // ap1->bitmap[1] = 0;
        ap1->length = 8*1+1;

        netconv_p4_header(h);
        netconv_p4_add_table_entry(te);
        netconv_p4_field_match_exact(exact);
        netconv_p4_action(a1);
        netconv_p4_action_parameter(ap1);
        send_p4_msg(c, buffer, 2048);
        usleep(1200);
    }
}
void fill_mf_exact_table(uint8_t dest_guid[4],uint8_t nxtHopMac[6],uint8_t port)
{
    char buffer[2048]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap,*ap2;
    struct p4_field_match_exact* exact;
    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "IngressImpl.mf_exact_0");

    exact = add_p4_field_match_exact(te, 2048);
    strcpy(exact->header.name, "mf.dest_guid");
    memcpy(exact->bitmap, dest_guid, 4);
    exact->length = 4*8+0;

    a = add_p4_action(h, 2048);
    strcpy(a->description.name, "IngressImpl.mf_set_egress_port");

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
void fill_ipv6_exact_table(uint8_t ipv6[8],uint8_t dstmac[6],uint8_t port)
{
    char buffer[2048]; /* TODO: ugly */
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap,*ap2;
    struct p4_field_match_exact* exact;

    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "IngressImpl.ipv6_exact_0");

    exact = add_p4_field_match_exact(te,2048);
    strcpy(exact->header.name,"ipv6.dstAddr");
    memcpy(exact->bitmap,ipv6,16);
    exact->length = 8*16+0;

    a = add_p4_action(h,2048);
    strcpy(a->description.name,"IngressImpl.ipv6_forward");

    ap = add_p4_action_parameter(h,a,2048);
    strcpy(ap->name,"dstmac");
    memcpy(ap->bitmap, dstmac, 6);
    ap->length = 6*8+0;

    ap2 = add_p4_action_parameter(h, a, 2048);
    strcpy(ap2->name, "port");
    memcpy(ap2->bitmap, &port, 1);
    // ap2->bitmap = port;
    // ap2->bitmap[1] = 0;
    ap2->length = 1*8+1;

    netconv_p4_header(h);
    netconv_p4_add_table_entry(te);
    netconv_p4_field_match_exact(exact);
    netconv_p4_action(a);
    netconv_p4_action_parameter(ap);
    netconv_p4_action_parameter(ap2);
    send_p4_msg(c, buffer, 2048);
    usleep(1200);
}

void fill_ipv4_lpm_table(uint8_t ip[4], uint8_t prefix, uint8_t dstmac[6], uint8_t port)
{
    char buffer[2048];
    struct p4_header *h;
    struct p4_add_table_entry *te;
    struct p4_action *a;
    struct p4_action_parameter *ap, *ap2;
    struct p4_field_match_lpm *lpm;
    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer, 0, 2048);
    strcpy(te->table_name, "IngressImpl.ipv4_lpm_0");

    FILE *fp6;
    fp6 = fopen("/opt/Workspace/P4/ppk/l3dst_ip_log.txt", "a");
    if (fp6)
    {
        fprintf(fp6, "ipv4:{%d}:{%d}:{%d}:{%d}:{%d}", ip[0], ip[1], ip[2], ip[3],port);
        fprintf(fp6, "\n");
        fclose(fp6);
    }

    lpm = add_p4_field_match_lpm(te, 2048);
    strcpy(lpm->header.name, "ipv4.dstAddr");
    memcpy(lpm->bitmap, ip, 4);
    lpm->prefix_length = prefix;

    a = add_p4_action(h, 2048);
    strcpy(a->description.name, "IngressImpl.ipv4_forward");

    ap = add_p4_action_parameter(h, a, 2048);
    strcpy(ap->name, "dstmac");
    memcpy(ap->bitmap, dstmac, 6);
    ap->length = 6 * 8 + 0;

    ap2 = add_p4_action_parameter(h, a, 2048);
    strcpy(ap2->name, "port");
    memcpy(ap2->bitmap, &port, 1);
    ap2->bitmap[0] = port;
    ap2->bitmap[1] = 0;

    printf("NH-1\n");
    netconv_p4_header(h);
    netconv_p4_add_table_entry(te);
    netconv_p4_field_match_lpm(lpm);
    netconv_p4_action(a);
    netconv_p4_action_parameter(ap);
    netconv_p4_action_parameter(ap2);

    send_p4_msg(c, buffer, 2048);
    usleep(1200);
}
void set_default_action_ipv4_lpm()
{
    char buffer[2048];
    struct p4_header *h;
    struct p4_set_default_action *sda;
    struct p4_action *a;

    h = create_p4_header(buffer, 0, sizeof(buffer));

    sda = create_p4_set_default_action(buffer, 0, sizeof(buffer));
    strcpy(sda->table_name, "IngressImpl.ipv4_lpm_0");
    a = &(sda->action);
    strcpy(a->description.name, "_drop");

    netconv_p4_header(h);
    netconv_p4_set_default_action(sda);
    netconv_p4_action(a);

    send_p4_msg(c, buffer, sizeof(buffer));
}

void set_default_action_geo_exact()
{
    char buffer[2048];
    struct p4_header* h;
    struct p4_set_default_action* sda;
    struct p4_action* a;


    h = create_p4_header(buffer, 0, sizeof(buffer));

    sda = create_p4_set_default_action(buffer,0,sizeof(buffer));
    strcpy(sda->table_name, "IngressImpl.geo_exact_0");

    a = &(sda->action);
    strcpy(a->description.name, "geo_drop");

    netconv_p4_header(h);
    netconv_p4_set_default_action(sda);
    netconv_p4_action(a);

    send_p4_msg(c, buffer, sizeof(buffer));
}

void set_default_action_ipv6_exact()
{
    char buffer[2048];
    struct p4_header* h;
    struct p4_set_default_action* sda;
    struct p4_action* a;


    h = create_p4_header(buffer, 0, sizeof(buffer));

    sda = create_p4_set_default_action(buffer,0,sizeof(buffer));
    strcpy(sda->table_name, "IngressImpl.ipv6_exact_0");

    a = &(sda->action);
    strcpy(a->description.name, "_drop");

    netconv_p4_header(h);
    netconv_p4_set_default_action(sda);
    netconv_p4_action(a);

    send_p4_msg(c, buffer, sizeof(buffer));
}
void set_default_action_mf_exact()
{
    char buffer[2048];
    struct p4_header* h;
    struct p4_set_default_action* sda;
    struct p4_action* a;


    h = create_p4_header(buffer, 0, sizeof(buffer));

    sda = create_p4_set_default_action(buffer,0,sizeof(buffer));
    strcpy(sda->table_name, "IngressImpl.mf_exact_0");

    a = &(sda->action);
    strcpy(a->description.name, "mf_drop");

    netconv_p4_header(h);
    netconv_p4_set_default_action(sda);
    netconv_p4_action(a);

    send_p4_msg(c, buffer, sizeof(buffer));
}

void dhf(void* b) {
       printf("Unknown digest received\n");
}

void init_simple() {
	uint8_t ip[4] = {10,0,99,99};
	uint8_t ip2[4] = {10,0,98,98};
	uint8_t ipv6[8] = {};
    uint8_t mask[4] = {255,255,255,255};
    uint8_t dstmac[6] = {0x11,0x22,0x33,0x44,0x55,0x66};
	uint8_t port = 1;
    // uint8_t ttl = 0;
    // uint8_t hoplimit = 0;
	fill_geo_exact_table(ip2,mask,dstmac,port);
	fill_ipv6_exact_table(ipv6,dstmac,port);
}


int read_config_from_file(char *filename)
{
	char line[100];
	uint8_t ipv6[8];
    uint8_t ip[4];
    // int n = -1;
    // int n = -1;
	uint8_t port;
    // uint16_t port_ipv6;
    uint8_t prefix;
	uint8_t mask[4];
    uint8_t dstmac[6];
    uint8_t nxtHopMac[6];
    uint8_t dest_guid[4];
    uint8_t port_num;
    uint8_t dst_node;
    uint16_t isvalid1[6];
    uint8_t flag;
    u_int32_t bitcast;
    // uint8_t ttl;
    // uint8_t hoplimit;
	char dummy;
    // FILE *fp;
     FILE *fp3;
	FILE *f;
	f = fopen(filename,"r");
	if (f == NULL) return -1;

	int line_index = 0;
	while (fgets(line,sizeof(line),f)){
		line[strlen(line)-1] = '\0';
		line_index++;
		printf("Sor:%d.",line_index);
	
		if (line[0] == 'G'){
            if(10 == sscanf(line,"%c %x %x %x %x %x %x %x %d %d"
            ,&dummy,&isvalid1[0],&isvalid1[1],&isvalid1[2],&isvalid1[3],&isvalid1[4],&isvalid1[5],&bitcast,&port,&flag)){
                fill_geo_exact_table(isvalid1,flag,port,bitcast);
            }
			else{
                fclose(f);
                return -1;
			}
		}
		else if (line[0] == 'S'){
			if(24 == sscanf(line,"%c %2hhx%2hhx:%2hhx%2hhx:%2hhx%2hhx:%2hhx%2hhx:%2hhx%2hhx:%2hhx%2hhx:%2hhx%2hhx:%2hhx%2hhx %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %hhd",&dummy,&ipv6[0],&ipv6[1],&ipv6[2],&ipv6[3],&ipv6[4],&ipv6[5],&ipv6[6],&ipv6[7],&ipv6[8],&ipv6[9],&ipv6[10],&ipv6[11],&ipv6[12],&ipv6[13],&ipv6[14],&ipv6[15],&dstmac[0],&dstmac[1],&dstmac[2],&dstmac[3],&dstmac[4],&dstmac[5],&port))
			{
            //     fp = fopen("/home/it-34/t4/t4p4s/ipv6_log.txt","a+");
            //     fprintf(fp,"xixihaha");
            //     if(fp){
			// 	fprintf(fp,"ipv6 dstmac:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}  {%d}:{%d}:{%d}:{%d}:{%d}:{%d}",ipv6[0],ipv6[1],ipv6[2],ipv6[3],ipv6[4],ipv6[5],ipv6[6],ipv6[7],dstmac[0],dstmac[1],dstmac[2],dstmac[3],dstmac[4],dstmac[5]);
            //     fprintf(fp,"\n");
            //     fclose(fp);
            // }
				fill_ipv6_exact_table(ipv6,dstmac,port);
			}
			else{
                fclose(f);
                return -1;
			}
		}
		else if(line[0] == 'M'){

			if (12 == sscanf(line,"%c %hhx %hhx %hhx %hhx %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %hhd",
				&dummy,&dest_guid[0],&dest_guid[1],&dest_guid[2],&dest_guid[3],&nxtHopMac[0],&nxtHopMac[1],&nxtHopMac[2],&nxtHopMac[3],&nxtHopMac[4],&nxtHopMac[5],&port))
			{
				fill_mf_exact_table(dest_guid,nxtHopMac,port);
			}
			else{
                fclose(f);
                return -1;
			}
		}
        else if (line[0] == 'E'){
            if (13 == sscanf(line, "%c %hhd.%hhd.%hhd.%hhd %hhd %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %hhd",
                             &dummy, &ip[0], &ip[1], &ip[2], &ip[3], &prefix, &dstmac[0], &dstmac[1], &dstmac[2], &dstmac[3], &dstmac[4], &dstmac[5], &port))
            {
                // fp3 = fopen("/home/ubuntu/log/l3dst_ipv4_log.txt","a");
                // 		if(fp3){
                //      fprintf(fp3,"ipv4:{%d}{%d}{%d}{%d},{%d}{%d}{%d}{%d}{%d}{%d}",ip[0],ip[1],ip[2],ip[3],dstmac[0],dstmac[1],dstmac[2],dstmac[3],dstmac[4],dstmac[5]);
                //      		fprintf(fp3,"\n");
                //      		fclose(fp3);
                //  }
                fill_ipv4_lpm_table(ip, prefix, dstmac, port);
                // fp3 = fopen("/home/ubuntu/log/l3dst_ip_log.txt", "a");
                // if (fp3)
                //{
                //     fprintf(fp3, "****************");
                //     fprintf(fp3, "\n");
                //     fclose(fp3);
                // }
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
	set_default_action_geo_exact();
	set_default_action_ipv6_exact();
    set_default_action_mf_exact();
    //set_default_action_ipv4_lpm();
	if (read_config_from_file(fn)<0) {
        printf("Read config from file failed\n");
    }
}

int main(int argc, char* argv[])
{

	if (argc>1) {
		if (argc!=2) {
			return -1;
		}
        fn = argv[1];
        
		c = create_controller_with_init(11111, 3, dhf, init_complex);
	}
	else {
        // fp = fopen("/home/it-34/t4/t4p4s/ipv6_log.txt","a");
        // if(fp){
        //         fprintf(fp,"notify____00");
        //         fprintf(fp,"\n");
        //         fclose(fp);
        //     }
		c = create_controller_with_init(11111, 3, dhf, init_simple);
	}
    // fp = fopen("/home/it-34/t4/t4p4s/1_log.txt","a");
    //     if(fp){
    //             fprintf(fp,"notify____11");
    //             fprintf(fp,"\n");
    //             fclose(fp);
    //         }
    notify_controller_initialized();
    // fp = fopen("/home/it-34/t4/t4p4s/1_log.txt","a");
    //     if(fp){
    //             fprintf(fp,"notify____22");
    //             fprintf(fp,"\n");
    //             fclose(fp);
    //         }

    // fp = fopen("/home/it-34/t4/t4p4s/1_log.txt","a");
    //     if(fp){
    //             fprintf(fp,"notify____33");
    //             fprintf(fp,"\n");
    //             fclose(fp);
    //         }
	execute_controller(c);
    // fp = fopen("/home/it-34/t4/t4p4s/1_log.txt","a");
    //     if(fp){
    //             fprintf(fp,"notify____44");
    //             fprintf(fp,"\n");
    //             fclose(fp);
    //         }

	destroy_controller(c);
    // fp = fopen("/home/it-34/t4/t4p4s/1_log.txt","a");
    //     if(fp){
    //             fprintf(fp,"notify____66");
    //             fprintf(fp,"\n");
    //             fclose(fp);
    //         }

	return 0;
}
