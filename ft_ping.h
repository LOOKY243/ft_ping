#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>

typedef struct s_flags
{
    int count;
    int verbose;
} t_flags;

typedef struct s_global_info
{
    char      **hosts;
    char       *ip;
    double     *rtts;
    int         sent_packets;
    int         received_packets;
    int         packet_loss;
    double      total_time;
    int         sockfd;
    double      shortest_rtt;
    double      longest_rtt;
    double      average_rtt;
} t_global_info;

extern t_global_info info;

int        create_socket(void);
void       print_intro(struct sockaddr_in *dest_addr, int verbose);
void       cleanup(void);
int        isnumeric(const char *str);
char     **add_hosts_array(char **hosts, char *new_host, int size);
void       initiate_info(void);
void       print_stats(void);
int        parse_args(int argc, char **argv, t_flags *flags);
double    *add_rtts_array(int size, double new_rtt);
uint16_t   checksum(void *data, size_t len);
void       send_icmp_echo(int sockfd, struct sockaddr_in *dest_addr, uint16_t seq);
double     get_current_time(void);
void       set_packet_header(struct icmphdr *packet, int seq);
void       c_signal_handler(int signal);
void       resolve_hostname(const char *hostname, struct sockaddr_in *dest_addr);
void       receive_icmp_reply(int sockfd, struct sockaddr_in *dest_addr, uint16_t seq, double send_time);

#endif
