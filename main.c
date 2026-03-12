#include "ft_ping.h"

t_global_info info;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        return 1;
    }
    struct sockaddr_in dest_addr;
    info.sockfd = create_socket();

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    resolve_hostname(argv[1], &dest_addr);
    int seq = 1;
    info.ip = argv[1];
    signal(SIGINT, c_signal_handler);
    printf("PING %s (%s): 56 data bytes\n", argv[1], inet_ntoa(dest_addr.sin_addr));
    info.total_time = get_current_time();
    while (1)
    {
        send_icmp_echo(info.sockfd, &dest_addr, seq);
        seq++;
    }
    close(info.sockfd);
    return 0;
}