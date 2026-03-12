#include "ft_ping.h"

double calculate_stddev(void)
{
    double sum = 0.0;
    double mean = info.average_rtt / info.received_packets;

    for (int i = 0; i < info.received_packets; i++)
    {
        sum += pow(info.rtts[i] - mean, 2);
    }
    return sqrt(sum / info.received_packets);
}

void print_stats(void)
{
    info.packet_loss = (info.sent_packets - info.received_packets) * 100 / info.sent_packets;
    info.total_time = get_current_time() - info.total_time;

    printf("--- %s ping statistics ---\n", info.ip);
    printf("%d packets transmitted, %d packets received, %d%% packet loss\nround-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", \
        info.sent_packets, info.received_packets, info.packet_loss, info.shortest_rtt, info.average_rtt / info.received_packets, info.longest_rtt, calculate_stddev());
}

double *add_rtts_array(int size, double new_rtt)
{
    double *new_array = malloc(size * sizeof(double));
    if (!new_array)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < size - 1; i++)
    {
        new_array[i] = info.rtts[i];
    }
    new_array[size - 1] = new_rtt;
    free(info.rtts);
    return new_array;
}

void c_signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        print_stats();
        close(info.sockfd);
        free(info.rtts);
        exit(EXIT_SUCCESS);
    }
}

double get_current_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (double)time.tv_sec * 1000.0 + (double)time.tv_usec / 1000.0;
}

void set_packet_header(struct icmphdr *packet, int seq)
{
    packet->type = ICMP_ECHO;
    packet->code = 0;
    packet->un.echo.id = getpid() & 0xFFFF;
    packet->un.echo.sequence = seq;
    packet->checksum = checksum(packet, sizeof(packet));
}

int create_socket()
{
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}