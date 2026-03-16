#include "ft_ping.h"

static int h_flag(void)
{
    printf("Usage: ping [OPTION...] HOST ...\n"
           "Send ICMP ECHO_REQUEST packets to network hosts.\n\n"
           "Options valid for all request types:\n"
           "  -c, --count=N       Stop after sending N ECHO_REQUEST packets.\n"
           "  -h, --help          Display this help and exit.\n"
           "  -v, --verbose       Verbose output.\n");
    return 1;
}

void print_intro(struct sockaddr_in *dest_addr, int verbose)
{
    if (verbose)
    {
        printf("PING %s (%s): 56 data bytes, id %d = %d\n",
               info.hosts[0], inet_ntoa(dest_addr->sin_addr), getpid(), getpid() & 0xFFFF);
    }
    else
    {
        printf("PING %s (%s): 56 data bytes\n",
               info.hosts[0], inet_ntoa(dest_addr->sin_addr));
    }
}

static void v_flag(t_flags *flags)
{
    flags->verbose = 1;
}

static void c_flag(t_flags *flags, char *arg)
{
    if (!isnumeric(arg))
    {
        fprintf(stderr, "ft_ping: invalid value (`%s' near `%s')\n", arg, arg);
        exit(1);
    }
    int count = atoi(arg);
    flags->count = (count < 0) ? 0 : count;
}

int parse_args(int argc, char **argv, t_flags *flags)
{
    flags->count   = 0;
    flags->verbose = 0;
    int ips        = 1;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--usage") == 0)
        {
            cleanup();
            return h_flag();
        }
        else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0)
        {
            if (i + 1 < argc)
            {
                c_flag(flags, argv[++i]);
            }
            else
            {
                fprintf(stderr, "Option '%s' requires an argument.\n", argv[i]);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
            v_flag(flags);
        else if (argv[i][0] != '-')
            info.hosts = add_hosts_array(info.hosts, argv[i], ips++);
        else
        {
            fprintf(stderr, "ft_ping: invalid option -- '%s'\n", argv[i]);
            printf("Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
            return 1;
        }
    }
    return 0;
}
