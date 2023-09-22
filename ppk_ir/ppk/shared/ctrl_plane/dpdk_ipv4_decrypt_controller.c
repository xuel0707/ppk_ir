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
FILE *fp;
controller c;

void fill_ipv4_lpm_table(uint8_t ip[4],uint8_t prefix,uint8_t dstmac[6],uint8_t port)
{
    char buffer[2048]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap,*ap2;
    struct p4_field_match_lpm* lpm;
    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "MyIngress.ipv4_lpm_0");

    //  FILE *fp6;
    //  fp6 = fopen("/home/zhaoxing/log/l3dst_ip_log.txt","a");
    //      if(fp6){
	//  	 fprintf(fp6,"ipv4:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}",ip[0],ip[1],ip[2],ip[3],ip[4],ip[5],ip[6],ip[7]);
    //              fprintf(fp6,"\n");
    //              fclose(fp6);
    //          }

    lpm = add_p4_field_match_lpm(te, 2048);
    strcpy(lpm->header.name, "ipv4.dstAddr");
    memcpy(lpm->bitmap, ip, 4);
    lpm->prefix_length = prefix;

    a = add_p4_action(h, 2048);
    strcpy(a->description.name, "MyIngress.ipv4_forward");

    ap = add_p4_action_parameter(h,a,2048);
    strcpy(ap->name,"dstmac");
    memcpy(ap->bitmap,dstmac,6);
    ap->length = 6*8+0;

    ap2 = add_p4_action_parameter(h, a, 2048);
    strcpy(ap2->name, "port");
    memcpy(ap2->bitmap, &port, 4);
    // ap2->bitmap = port;
    // ap2->bitmap[1] = 0;
    ap2->length = 4*8+0;

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

void fill_ipv4_decry_table(uint8_t src_ip[4],uint8_t dst_ip[4],uint8_t flag,uint8_t port,uint8_t prefix){
    char buffer[4096]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap,* ap1,* ap2,* ap3,* ap4,* ap5;
    struct p4_field_match_exact* exact,*exact1;

    h = create_p4_header(buffer, 0, 4096);
    te = create_p4_add_table_entry(buffer,0,4096);
    strcpy(te->table_name, "MyIngress.ipv4_spd_0");

    exact = add_p4_field_match_exact(te,4096);
    strcpy(exact->header.name,"ipv4.srcAddr");
    memcpy(exact->bitmap,&src_ip[0],4);

    exact1 = add_p4_field_match_exact(te,4096);
    strcpy(exact1->header.name,"ipv4.dstAddr");
    memcpy(exact1->bitmap,&dst_ip[0],4);

    a = add_p4_action(h,4096);
    strcpy(a->description.name,"MyIngress.ipv4_decry");

    ap3 = add_p4_action_parameter(h, a, 4096);
    strcpy(ap3->name, "name1");
    memcpy(ap3->bitmap, &flag, 1);
    // // ap->bitmap = port;
    // // ap->bitmap[1] = 0;
    ap3->length = 8*1+0;

    ap4 = add_p4_action_parameter(h, a, 4096);
    strcpy(ap4->name, "name2");
    memcpy(ap4->bitmap, &port, 1);
    // // ap->bitmap = port;
    // // ap->bitmap[1] = 0;
    ap4->length = 8*1+0;

    ap5 = add_p4_action_parameter(h, a, 4096);
    strcpy(ap5->name, "name3");
    memcpy(ap5->bitmap, &prefix, 1);
    // // ap->bitmap = port;
    // // ap->bitmap[1] = 0;
    ap5->length = 8*1+0;

    netconv_p4_header(h);
    netconv_p4_add_table_entry(te);
    netconv_p4_field_match_exact(exact);
    netconv_p4_field_match_exact(exact1);
    netconv_p4_action(a);
    netconv_p4_action_parameter(ap3);
    netconv_p4_action_parameter(ap4);
    netconv_p4_action_parameter(ap5);
    send_p4_msg(c, buffer, 4096);
    usleep(1200);
    
}
void fill_mf_decry_table(uint16_t isvalid[4],uint8_t flag,uint8_t port,uint8_t prefix){
    char buffer[4096]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap,* ap1,* ap2,* ap3,* ap4,* ap5;
    struct p4_field_match_exact* exact,*exact1;

    h = create_p4_header(buffer, 0, 4096);
    te = create_p4_add_table_entry(buffer,0,4096);
    strcpy(te->table_name, "MyIngress.mf_spd_0");

    exact = add_p4_field_match_exact(te,4096);
    strcpy(exact->header.name,"hdr.mf.src_guid");
    memcpy(exact->bitmap,&isvalid[0],4);

    exact1 = add_p4_field_match_exact(te,4096);
    strcpy(exact1->header.name,"hdr.mf.dest_guid");
    memcpy(exact1->bitmap,&isvalid[2],4);

    a = add_p4_action(h,4096);
    strcpy(a->description.name,"MyIngress.mf_decry");

    ap3 = add_p4_action_parameter(h, a, 4096);
    strcpy(ap3->name, "name1");
    memcpy(ap3->bitmap, &flag, 1);
    // // ap->bitmap = port;
    // // ap->bitmap[1] = 0;
    ap3->length = 8*1+0;

    ap4 = add_p4_action_parameter(h, a, 4096);
    strcpy(ap4->name, "name2");
    memcpy(ap4->bitmap, &port, 1);
    // // ap->bitmap = port;
    // // ap->bitmap[1] = 0;
    ap4->length = 8*1+0;

    ap5 = add_p4_action_parameter(h, a, 4096);
    strcpy(ap5->name, "name3");
    memcpy(ap5->bitmap, &prefix, 1);
    // // ap->bitmap = port;
    // // ap->bitmap[1] = 0;
    ap5->length = 8*1+0;

    netconv_p4_header(h);
    netconv_p4_add_table_entry(te);
    netconv_p4_field_match_exact(exact);
    netconv_p4_field_match_exact(exact1);
    netconv_p4_action(a);
    netconv_p4_action_parameter(ap3);
    netconv_p4_action_parameter(ap4);
    netconv_p4_action_parameter(ap5);
    send_p4_msg(c, buffer, 4096);
    usleep(1200);
    
}
void fill_dest_guid_exact_table(uint8_t dest_guid[4],uint8_t nxtHopMac[6],uint8_t port)
{
    char buffer[2048]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap,*ap2;
    struct p4_field_match_exact* exact;
    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "MyIngress.dest_guid_exact_0");

    //  FILE *fp6;
    //  fp6 = fopen("/home/it-34/log/l3dst_ip_log.txt","a");
    //      if(fp6){
	//  	 fprintf(fp6,"ipv4:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}:{%d}",nxtHopMac[0],nxtHopMac[1],nxtHopMac[2],nxtHopMac[3],nxtHopMac[4],nxtHopMac[5],dest_guid[6],dest_guid[7]);
    //              fprintf(fp6,"\n");
    //              fclose(fp6);
    //          }

    exact = add_p4_field_match_exact(te, 2048);
    strcpy(exact->header.name, "hdr.mf.dest_guid");
    memcpy(exact->bitmap, dest_guid, 4);
    exact->length = 4*8+0;

    a = add_p4_action(h, 2048);
    strcpy(a->description.name, "MyIngress.dest_guid_forward");

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
void fill_eth_exact_table(uint8_t port,uint8_t dstmac[6],uint8_t srcmac[6],u_int16_t etherType,uint8_t mask[15]){
    char buffer[2048]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    // struct p4_action_parameter* ap,* ap1;
    struct p4_field_match_ternary* ternary,*ternary1,*ternary2,*ternary3;


    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "MyIngress.eth_exact_0");

    ternary = add_p4_field_match_ternary(te,2048);
    strcpy(ternary->header.name,"standard_metadata.ingress_port");
    memcpy(ternary->bitmap, &port, 1);
    memcpy(ternary->mask, &mask[0], 1);

    ternary1 = add_p4_field_match_ternary(te,2048);
    strcpy(ternary1->header.name,"hdr.ethernet.dstAddr");
    memcpy(ternary1->bitmap, dstmac, 6);
    memcpy(ternary1->mask, &mask[1], 6);

    ternary2 = add_p4_field_match_ternary(te,2048);
    strcpy(ternary2->header.name,"hdr.ethernet.srcAddr");
    memcpy(ternary2->bitmap, srcmac, 6);
    memcpy(ternary2->mask, &mask[7], 1);


    ternary3 = add_p4_field_match_ternary(te,2048);
    strcpy(ternary3->header.name,"hdr.ethernet.etherType");
    memcpy(ternary3->bitmap, &etherType, 2);
    memcpy(ternary->mask, &mask[13], 2);
    

    a = add_p4_action(h,2048);
    strcpy(a->description.name,"fwd2ONOS");

    netconv_p4_header(h);
    netconv_p4_add_table_entry(te);
    netconv_p4_field_match_ternary(ternary);
    netconv_p4_field_match_ternary(ternary1);
    netconv_p4_field_match_ternary(ternary2);
    netconv_p4_field_match_ternary(ternary3);
    netconv_p4_action(a);
    // netconv_p4_action_parameter(ap);
    send_p4_msg(c, buffer, 2048);
    usleep(1200);
    
}

void set_default_action_ipv4_lpm()
{
    char buffer[2048];
    struct p4_header* h;
    struct p4_set_default_action* sda;
    struct p4_action* a;


    h = create_p4_header(buffer, 0, sizeof(buffer));

    sda = create_p4_set_default_action(buffer,0,sizeof(buffer));
    strcpy(sda->table_name, "ipv4_lpm_0");

    a = &(sda->action);
    strcpy(a->description.name, "_drop");

    netconv_p4_header(h);
    netconv_p4_set_default_action(sda);
    netconv_p4_action(a);

    send_p4_msg(c, buffer, sizeof(buffer));
}

void set_default_action_geo_ternary()
{
    char buffer[2048];
    struct p4_header* h;
    struct p4_set_default_action* sda;
    struct p4_action* a;


    h = create_p4_header(buffer, 0, sizeof(buffer));

    sda = create_p4_set_default_action(buffer,0,sizeof(buffer));
    strcpy(sda->table_name, "geo_ternary_0");

    a = &(sda->action);
    strcpy(a->description.name, "_drop");

    netconv_p4_header(h);
    netconv_p4_set_default_action(sda);
    netconv_p4_action(a);

    send_p4_msg(c, buffer, sizeof(buffer));
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

	// fill_ipv4_lpm_table(ip, 24, dstmac,port);
	// fill_geo_ternary_table(ip2,mask,dstmac,port);

}


void dhf(void* b) {
       printf("Unknown digest received\n");
}

int read_config_from_file(char *filename)
{
	char line[100];
	uint8_t ip[4];
    uint8_t ip2[4];
	uint8_t ipv6[8];
    uint8_t spi[4];
    uint8_t sAdrress[4];
    uint8_t dAddress[4];
    uint16_t isvalid1[6];
    uint16_t isvalid2[4];
    uint8_t dest_guid[4];
    uint8_t nxtHopMac[6];
    uint8_t flag;
    // int n = -1;
	uint8_t port;
    uint8_t prefix;
    uint8_t prefix2;
	uint8_t mask[4];
    uint8_t dstmac[6];
    uint32_t bitcast;
    uint32_t bitcast1;
    uint32_t bitcast2;

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
		if(line[0] == 'E'){
			if (13 == sscanf(line,"%c %hhd.%hhd.%hhd.%hhd %hhd %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %hhd",
				&dummy,&ip[0],&ip[1],&ip[2],&ip[3],&prefix,&dstmac[0],&dstmac[1],&dstmac[2],&dstmac[3],&dstmac[4],&dstmac[5],&port))
			{
				//fp3 = fopen("/home/it-34/log/ipv4_log.txt","a");
                		/*if(fp3){
				     fprintf(fp3,"ipv4:{%d}{%d}{%d}{%d},{%d}{%d}{%d}{%d}{%d}{%d}",ip[0],ip[1],ip[2],ip[3],dstmac[0],dstmac[1],dstmac[2],dstmac[3],dstmac[4],dstmac[5]);
                     		fprintf(fp3,"\n");
                     		fclose(fp3);
                 }*/
				fill_ipv4_lpm_table(ip,prefix,dstmac,port);
			}
			else{
                fclose(f);
                return -1;
			}
		}
        if(line[0]=='B'){
            if(12 == sscanf(line,"%c %hhd.%hhd.%hhd.%hhd %hhd.%hhd.%hhd.%hhd %d %d %d"
            ,&dummy,&ip[0],&ip[1],&ip[2],&ip[3],&ip2[0],&ip2[1],&ip2[2],&ip2[3],&flag,&port,&prefix)){
                fill_ipv4_decry_table(ip,ip2,flag,port,prefix); 
            }
            else{
                fclose(f);
                return -1;
            }
        }
        if(line[0]=='C'){
            if(8 == sscanf(line,"%c %x %x %x %x %d %d %d"
            ,&dummy,&isvalid2[0],&isvalid2[1],&isvalid2[2],&isvalid2[3],&flag,&port,&prefix)){
                fill_mf_decry_table(isvalid2,flag,port,prefix); 
            }
            else{
                fclose(f);
                return -1;
            }
        }
        if(line[0] == 'M'){

                if (12 == sscanf(line,"%c %hhx %hhx %hhx %hhx %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %hhd",
                    &dummy,&dest_guid[0],&dest_guid[1],&dest_guid[2],&dest_guid[3],&nxtHopMac[0],&nxtHopMac[1],&nxtHopMac[2],&nxtHopMac[3],&nxtHopMac[4],&nxtHopMac[5],&port))
                {
                    fill_dest_guid_exact_table(dest_guid,nxtHopMac,port);
                }
                else{
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
	set_default_action_ipv4_lpm();
	set_default_action_geo_ternary();

	if (read_config_from_file(fn)<0) {
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
