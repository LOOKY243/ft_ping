#include "ft_ping.h"

uint16_t checksum(void *data, size_t len)
{
    uint16_t *buf = data;
    uint32_t sum = 0;
    for (size_t i = 0; i < len / 2; i++)
    {
        sum += buf[i];
    }
    if (len % 2)
    {
        sum += ((uint8_t *)data)[len - 1];
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

void receive_icmp_reply(int sockfd, struct sockaddr_in *dest_addr, uint16_t seq, double send_time)
{
    char buffer[1024];
    double recv_time;
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    
    ssize_t recv_bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len); 
    if (recv_bytes < 0)
    {
        return ;
    }
    recv_time = get_current_time();
    struct iphdr *ip_header = (struct iphdr *)buffer;
    struct icmphdr *icmp_header = (struct icmphdr *)(buffer + ip_header->ihl * 4);
    if (icmp_header->type == ICMP_ECHOREPLY && icmp_header->un.echo.id == (getpid() & 0xFFFF) && icmp_header->un.echo.sequence == seq)
    {
        printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", inet_ntoa(dest_addr->sin_addr), seq, ip_header->ttl, (double)(recv_time - send_time));
        info.received_packets++;
        if (info.shortest_rtt == 0 || recv_time - send_time < info.shortest_rtt)
            info.shortest_rtt = recv_time - send_time;
        if (recv_time - send_time > info.longest_rtt)
            info.longest_rtt = recv_time - send_time;
        info.rtts = add_rtts_array(info.received_packets, recv_time - send_time);
        info.average_rtt += recv_time - send_time;
    }
    double elapsed_ms = recv_time - send_time;
    double remaining_ms = 1000.0 - elapsed_ms;

    if (remaining_ms > 0)
        usleep(remaining_ms * 1000);
}

void send_icmp_echo(int sockfd, struct sockaddr_in *dest_addr, uint16_t seq)
{
    char send_packet[64];
    struct icmphdr *send_header;
    double send_time;

    send_header = (struct icmphdr *)send_packet;
    memset(send_packet, 0, sizeof(send_packet));
    set_packet_header(send_header, seq);
    send_time = get_current_time();
    ssize_t sent_bytes = sendto(sockfd, &send_packet, sizeof(send_packet), 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));
    if (sent_bytes < 0)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    info.sent_packets++;
    receive_icmp_reply(sockfd, dest_addr, seq, send_time);
}

void resolve_hostname(const char *hostname, struct sockaddr_in *dest_addr)
{
    struct addrinfo *result;
    struct addrinfo hints;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
    {
        fprintf(stderr, "ft_ping: %s: Name or service not known\n", hostname);
        exit(EXIT_FAILURE);
    }
    dest_addr->sin_addr = ((struct sockaddr_in *)result->ai_addr)->sin_addr;
    freeaddrinfo(result);
}