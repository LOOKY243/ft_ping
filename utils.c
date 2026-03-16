#include "ft_ping.h"

void initiate_info(void)
{
    info.sent_packets = 0;
    info.received_packets = 0;
    info.shortest_rtt = 0;
    info.longest_rtt = 0;
    info.average_rtt = 0;
    free(info.rtts);
    info.rtts = NULL;
}

void cleanup(void)
{
    free(info.rtts);
    info.rtts = NULL;
    for (int i = 0; info.hosts && info.hosts[i]; i++)
        free(info.hosts[i]);
    free(info.hosts);
    info.hosts = NULL;
}

int isnumeric(const char *str)
{
    int i;
    
    if (str[0] == '-' || str[0] == '+')
        i = 1;
    else
        i = 0;
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return 0;
        i++;
    }
    return 1;
}

static double calculate_stddev(void)
{
    double avg = info.average_rtt / info.received_packets;
    double sum = 0;

    for (int i = 0; i < info.received_packets; i++)
        sum += pow(info.rtts[i] - avg, 2);
    return sqrt(sum / info.received_packets);
}

void print_stats(void)
{
    double elapsed = get_current_time() - info.total_time;

    if (info.sent_packets > 0)
        info.packet_loss = (info.sent_packets - info.received_packets) * 100 / info.sent_packets;
    else
        info.packet_loss = 0;

    printf("--- %s ping statistics ---\n", info.ip);
    printf("%d packets transmitted, %d packets received, %d%% packet loss, time %.0f ms\n",
           info.sent_packets, info.received_packets, info.packet_loss, elapsed);

    if (info.received_packets > 0)
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
               info.shortest_rtt,
               info.average_rtt / info.received_packets,
               info.longest_rtt,
               calculate_stddev());
}

double *add_rtts_array(int size, double new_rtt)
{
    double *new_array = realloc(info.rtts, size * sizeof(double));
    if (!new_array)
    {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    new_array[size - 1] = new_rtt;
    return new_array;
}

char **add_hosts_array(char **hosts, char *new_host, int size)
{
    char **new_array = malloc((size + 1) * sizeof(char *));
    if (!new_array)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < size - 1; i++)
        new_array[i] = hosts[i];
    new_array[size - 1] = strdup(new_host);
    new_array[size] = NULL;
    free(hosts);
    return new_array;
}

void c_signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("\n");
        print_stats();
        close(info.sockfd);
        cleanup();
        exit(EXIT_SUCCESS);
    }
}

double get_current_time(void)
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (double)time.tv_sec * 1000.0 + (double)time.tv_usec / 1000.0;
}

void set_packet_header(struct icmphdr *packet, int seq)
{
    packet->type             = ICMP_ECHO;
    packet->code             = 0;
    packet->un.echo.id       = getpid() & 0xFFFF;
    packet->un.echo.sequence = seq;
    packet->checksum         = 0;
    packet->checksum         = checksum(packet, sizeof(struct icmphdr));
}

int create_socket(void)
{
    struct timeval timeout;
    timeout.tv_sec  = 1;
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
