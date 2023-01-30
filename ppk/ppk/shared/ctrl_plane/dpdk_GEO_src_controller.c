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

void fill_gbc_exact_table(uint16_t isvalid[4],uint8_t flag,uint8_t port,uint32_t bitcast){
    char buffer[2048]; 
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a,* a1, *a2;
    struct p4_action_parameter* ap,* ap1;
    struct p4_field_match_exact* exact,*exact1;

    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "MyIngress.gbc_src_exact_0");

    exact = add_p4_field_match_exact(te,2048);
    strcpy(exact->header.name,"hdr.gbc.lat");
    memcpy(exact->bitmap,&isvalid[0],4);

    exact1 = add_p4_field_match_exact(te,2048);
    strcpy(exact1->header.name,"hdr.gbc.longg");
    memcpy(exact1->bitmap,&isvalid[2],4);

    if(flag == 0){
        a = add_p4_action(h,2048);
        strcpy(a->description.name,"MyIngress.multicast");
        ap = add_p4_action_parameter(h, a, 2048);
        strcpy(ap->name, "bitcast");
        memcpy(ap->bitmap, &bitcast, 4);
        // // ap->bitmap = port;
        // // ap->bitmap[1] = 0;
        ap->length = 8*2+0;
        netconv_p4_header(h);
        netconv_p4_add_table_entry(te);
        netconv_p4_field_match_exact(exact);
        netconv_p4_field_match_exact(exact1);
        netconv_p4_action(a);
        netconv_p4_action_parameter(ap);
        send_p4_msg(c, buffer, 2048);
        usleep(1200);
    }

    if(flag == 1){
        a1 = add_p4_action(h,2048);
        strcpy(a1->description.name,"MyIngress.unicast");
        ap1 = add_p4_action_parameter(h, a1, 2048);
        strcpy(ap1->name, "port");
        memcpy(ap1->bitmap, &port, 1);
        // // ap1->bitmap = port;
        // // ap1->bitmap[1] = 0;
        ap1->length = 8*1+1;

        netconv_p4_header(h);
        netconv_p4_add_table_entry(te);
        netconv_p4_field_match_exact(exact);
        netconv_p4_field_match_exact(exact1);
        netconv_p4_action(a1);
        netconv_p4_action_parameter(ap1);
        send_p4_msg(c, buffer, 2048);
        usleep(1200);
    }

    // netconv_p4_header(h);
    // netconv_p4_add_table_entry(te);
    // netconv_p4_field_match_exact(exact);
    // netconv_p4_field_match_exact(exact1);
    // netconv_p4_field_match_exact(exact2);
    // netconv_p4_field_match_exact(exact3);
    // netconv_p4_action(a);
    // netconv_p4_action(a1);
    // netconv_p4_action(a2);
    // netconv_p4_action_parameter(ap);
    // send_p4_msg(c, buffer, 2048);
    // usleep(1200);
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

void dhf(void* b) {
       printf("Unknown digest received\n");
}

int read_config_from_file(char *filename)
{
    char line[100];
    uint8_t dstmac[6];
    uint8_t srcmac[6];
    // _Bool valid1;
    // _Bool valid2;
    // _Bool valid3;
    // _Bool valid4;
    uint8_t isvalid;
    uint16_t etherType;
    uint16_t isvalid1[6];
    uint8_t isvalid2;
    uint8_t flag;
    u_int32_t bitcast;
    uint8_t port;

    char dummy;
    FILE *f;
    f = fopen(filename,"r");
	if (f == NULL) return -1;

    int line_index = 0;
	while (fgets(line,sizeof(line),f)){
        line[strlen(line)-1] = '\0';
		line_index++;
		printf("Sor:%d.",line_index);
        if(line[0]=='A'){
            if(8 == sscanf(line,"%c %x %x %x %x %d %d %x"
            ,&dummy,&isvalid1[0],&isvalid1[1],&isvalid1[2],&isvalid1[3],&flag,&port,&bitcast)){
                fill_gbc_exact_table(isvalid1,flag,port,bitcast);
            }
            else{
                fclose(f);
                return -1;
            }
        }
        // else if(line[0]=='B'){
        //     // fp6 = fopen("/home/ndsc/log/fenpian.txt","a");
        // //  if(fp6){
	 	// //  fprintf(fp6,"bbb:%d",sscanf(line,"%c %hhd %hhx %hhx %hhx %hhx %hhx %hhd",&dummy,&isvalid,&isvalid1,&isvalid2,&isvalid3,&isvalid4,&isvalid5,&isvalid6));
        // //          fprintf(fp6,"\n");
        // //          fclose(fp6);
        // //      }
        //     if(5 == sscanf(line,"%c %hhd %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %x ",
        //     &dummy,&dstmac[0], &dstmac[1], &dstmac[2], &dstmac[3], &dstmac[4], &dstmac[5],&srcmac[0], &srcmac[1], &srcmac[2], &srcmac[3], &srcmac[4], &srcmac[5],&port,&isvalid1[0],&isvalid1[1],&isvalid1[2],&isvalid1[3],&isvalid1[4],&mask[0],&mask[1],&mask[2],&mask[3],&mask[4],&isvalid2)){
        //         // fp6 = fopen("/home/ndsc/log/fenpian.txt","a");
        // //  if(fp6){
	 	// //  fprintf(fp6,"bbb:%x %x",isvalid1[2],isvalid1[4]);
        // //          fprintf(fp6,"\n");
        // //          fclose(fp6);
        // //      }
        //         fill_fib_table_table(isvalid1);
                
        //     }
        //     else{
        //         fclose(f);
        //         return -1;
        //     }
        // }

    }
    fclose(f);
	return 0;
}



char* fn;
void init_complex(){
	// set_default_action_section_table();
    // set_default_action_count_table();
    // set_default_action_fib_table();
    // set_default_action_updatePit_table();

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
        printf("--------init field------");
	}

    notify_controller_initialized();

	execute_controller(c);

	destroy_controller(c);

	return 0;
}