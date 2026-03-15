#include "ft_ping.h"

int h_flag(void)
{
    printf("Usage: ping [OPTION...] HOST ...\nSend ICMP ECHO_REQUEST packets to network hosts.\n");
    printf("\nOptions valid for all request types:\n");
    printf("  -c, --count=N       Stop after sending N ECHO_REQUEST packets.\n");
    printf("  -h, --help          Display this help and exit.\n");
    printf("  -v, --verbose       Verbose output.\n");
    return 1;
}

void v_flag(t_flags *flags)
{
    flags->verbose = 1;
}

void c_flag(t_flags *flags, char *arg)
{
    int count;
    if (isnumeric(arg) == 0)
    {
        fprintf(stderr, "ft_ping: invalid value (`%s' near `%s'):'\n", arg, arg);
        exit(1);
    }
    count = atoi(arg);
    if (count < 0)
        flags->count = 0;
    else
        flags->count = count;
}

int parse_args(int argc, char **argv, t_flags *flags)
{
    flags->count = 0;
    flags->verbose = 0;
    int ips = 1;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0)
        {
            cleanup();
            return h_flag();
        }
        else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0)
        {
            if (i + 1 < argc)
            {
                c_flag(flags, argv[i + 1]);
                i++;
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
            // need to initiate a before calling the function
            info.hosts = add_hosts_array(info.hosts, argv[i], ips++);
    }
    return 0;
}