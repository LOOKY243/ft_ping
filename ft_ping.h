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


typedef struct s_global_info
{
    char *ip;
    double *rtts;
    int sent_packets;
    int received_packets;
    int packet_loss;
    int total_time;
    int sockfd;
    double shortest_rtt;
    double longest_rtt;
    double average_rtt;
} t_global_info;

extern t_global_info info;

int create_socket();
double *add_rtts_array(int size, double new_rtt);
uint16_t checksum(void *data, size_t len);
void send_icmp_echo(int sockfd, struct sockaddr_in *dest_addr, uint16_t seq);
double get_current_time(void);
void set_packet_header(struct icmphdr *packet, int seq);
void c_signal_handler(int signal);
void resolve_hostname(const char *hostname, struct sockaddr_in *dest_addr);

#endif