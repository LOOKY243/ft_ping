#include "ft_ping.h"

t_global_info info;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "ft_ping: missing host operand\n"
                        "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
        return 1;
    }

    t_flags             flags;
    struct sockaddr_in  dest_addr;

    if (parse_args(argc, argv, &flags))
        return 1;
    info.sockfd = create_socket();
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    signal(SIGINT, c_signal_handler);

    for (int i = 0; info.hosts && info.hosts[i]; i++)
    {
        resolve_hostname(info.hosts[i], &dest_addr);
        print_intro(&dest_addr, flags.verbose);
        initiate_info();
        info.ip = info.hosts[i];
        info.total_time = get_current_time();

        while (flags.count == 0 || info.sent_packets < flags.count)
            send_icmp_echo(info.sockfd, &dest_addr, info.sent_packets + 1);

        print_stats();
    }

    close(info.sockfd);
    cleanup();
    return 0;
}
