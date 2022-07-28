// Project: IPK - Packet sniffer
// Author: Michal Cibák - xcibak00

// INFO:
// version 1.1.0 of libpcap is needed for PCAP_NETMASK_UNKNOWN constant
// program must be started in a console window with admin privileges

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <pcap.h>
#include <sys/time.h>

#define MAX_FILTER_EXP_SIZE 100

struct parameters
{
    char *interface;
    long port;
    bool udp;
    bool tcp;
    long number;
};

struct parameters progpar = {NULL, -1, false, false, -1};

int string_to_long_int(char *string, long *number)
// converts string to long
// returns 0 if conversion is successful, 1 if string is not a number, 2 if number is out of range of long
{
    char *afternumber = NULL;
    errno = 0;
    *number = strtol(string, &afternumber, 0);
    if (!afternumber || afternumber == string || *afternumber) /// TODO - "!afternumber" and "afternumber == string" may be doing the same; which condition catches empty string?
        return 1;
    else if (errno)
        return 2;
    else
        return 0;
}

int argcheck(int argc, char *argv[])
// checks if program arguments are correct and used only once
//   fills globally available structure with values according to the arguments
//   prints error message in case of error
// returns 0 on successful check, 1 if there is error in program arguments (or any other error)
{
    for (int i = 1; i < argc; ++i)
    {
        // switch (argv[i]){
        // case "-i":
        if (strcmp(argv[i], "-i") == 0)
        {
            if (progpar.interface)
            {
                fprintf(stderr, "# Repeating -i argument.\n");
                return 1;
            }
            else if (++i < argc)
            {
                progpar.interface = argv[i];
            }
            else
            {
                fprintf(stderr, "# Missing value of -i argument.\n");
                return 1;
            }
        }
        // case "-p":
        else if (strcmp(argv[i], "-p") == 0)
        {
            if (progpar.port != -1)
            {
                fprintf(stderr, "# Repeating -p argument.\n");
                return 1;
            }
            else if (++i < argc)
            {
                if (string_to_long_int(argv[i], &progpar.port))
                {
                    fprintf(stderr, "# Value of -p argument could not be converted into a number.\n");
                    return 1;
                }
                else if (progpar.port < 0 || progpar.port > 65535)
                {
                    fprintf(stderr, "# Port number is out of range.\n");
                    return 1;
                }
            }
            else
            {
                fprintf(stderr, "# Missing value of -p argument.\n");
                return 1;
            }
        }
        // case "-u":
        // case "--udp":
        else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--udp") == 0)
        {
            progpar.udp = true;
        }
        // case "-t":
        // case "--tcp":
        else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tcp") == 0)
        {
            progpar.tcp = true;
        }
        // case "-n":
        else if (strcmp(argv[i], "-n") == 0)
        {
            if (progpar.number != -1)
            {
                fprintf(stderr, "# Repeating -n argument.\n");
                return 1;
            }
            else if (++i < argc)
            {
                if (string_to_long_int(argv[i], &progpar.number))
                {
                    fprintf(stderr, "# Value of -n argument could not be converted into a number.\n");
                    return 1;
                }
                else if (progpar.number < 0)
                {
                    fprintf(stderr, "# Number of packets can not be negative.\n");
                    return 1;
                }
            }
            else
            {
                fprintf(stderr, "# Missing value of -n argument.\n");
                return 1;
            }
        }
        // default:
        else // unknown argument
        {
            fprintf(stderr, "# Unknown argument %d: %s\n", argc, argv[i]);
            return 1;
        }
        // }
    }
    return 0;
}

void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
// prints captured packet in specified format
{
    // fprintf(stderr, "$ A packed was captured on interface %s with lenght of %d.\n", progpar.interface, header->len); /// DEBUG
    long seconds = header->ts.tv_sec % 60;
    long minutes = (header->ts.tv_sec % 3600) / 60;
    long hours = (header->ts.tv_sec % 86400) / 3600;
    printf("%ld:%ld:%ld.%ld\n", hours, minutes, seconds, header->ts.tv_usec);

    for (unsigned int i = 0; i < header->caplen; ++i)
    {
        if (i % 16 == 0)
        {
            printf("\n0x%04X:", i);
        }

        printf(" %s%02X", ((i % 8 == 0) && (i % 16 != 0)) ? " " : "", (unsigned int)packet[i]);
    }

    printf("\n\n");
}

int main(int argc, char *argv[])
{
    // program arguments checking, filling of globally available custom structure with converted values of the arguments
    if (argcheck(argc, argv)) // error in arguments
    {
        return 1;
    }

    pcap_if_t *interfaces;

    // find all interfaces on this device
    char errbuf[PCAP_ERRBUF_SIZE];
    if (pcap_findalldevs(&interfaces, errbuf))
    {
        fprintf(stderr, "# Error with finding interfaces: %s\n", errbuf);
        return 2;
    }

    pcap_if_t *interface;

    // additional check of loaded program parameters - behavior of program depends on which values are specified
    if (progpar.interface) // interface to be used was specified
    // check its existance
    {
        bool interface_exists = false;
        for (interface = interfaces; interface != NULL; interface = interface->next)
        {
            if (!strcmp(interface->name, progpar.interface))
            {
                interface_exists = true;
                break;
            }
        }

        if (interface_exists) // specified interface exists on this device
        // free the list of interfaces and scrap all references, set default values to unspecified program parameters
        {
            pcap_freealldevs(interfaces);
            interfaces = NULL;
            interface = NULL;

            if (!progpar.udp && !progpar.tcp) // neither udp or tcp packets were chosen
            // allow both udp and tcp packets
            {
                progpar.udp = true;
                progpar.tcp = true;
            }
            if (progpar.number == -1) // number of packets wasn't specified
            // choose a default value of 1
            {
                progpar.number = 1;
            }
        }
        else // specified interface doesn't exist on this device
        // free the list of interfaces and end program with error
        {
            fprintf(stderr, "# Interface %s isn't available on this device.\n", progpar.interface);
            pcap_freealldevs(interfaces);
            return 2;
        }
    }
    else // interface to be used was not specified
    // print existing interfaces and end program succesfully
    {
        if (interfaces) // interfaces were found
        {
            printf("Available interfaces:\n");
            for (interface = interfaces; interface != NULL; interface = interface->next)
            {
                printf("  %s - %s\n", interface->name, interface->description);
            }
            pcap_freealldevs(interfaces);
            return 0;
        }
        else // device has no interfaces
        {
            printf("No interfaces found on this device.\n");
            return 0;
        }
    }

    pcap_t *opened_interface;

    // open interface for listening
    opened_interface = pcap_open_live(progpar.interface, BUFSIZ, 1, 1000, errbuf); /// TODO - try different timeouts, e.g. 1 instead of 1000
    if (!opened_interface)
    {
        fprintf(stderr, "# Error with opening interface %s: %s\n", progpar.interface, errbuf);
        return 2;
    }

    // check if it supports communication via Ethernet
    if (pcap_datalink(opened_interface) != DLT_EN10MB)
    {
        fprintf(stderr, "# Interface %s provides unsupported link-layer header type.\n", progpar.interface);
        pcap_close(opened_interface);
        return(2);
    }

    // set filter for packets
    char filter_expression[MAX_FILTER_EXP_SIZE] = {0}; // will be automatically cast into const char * when used inpcap_compile()
    if (progpar.port != -1)
    {
        snprintf(filter_expression, sizeof(filter_expression), "%sport %ld", (progpar.udp && progpar.tcp) ? "" : (progpar.udp ? "udp " : "tcp "), progpar.port);
    }
    else
    {
        snprintf(filter_expression, sizeof(filter_expression), "%s", (progpar.udp && progpar.tcp) ? "tcp or udp" : (progpar.udp ? "udp" : "tcp"));
    }
    // fprintf(stderr, "$ Filter string: '%s'.\n", filter_expression); /// DEBUG
    struct bpf_program filter_program;
    if (pcap_compile(opened_interface, &filter_program, filter_expression, 1, PCAP_NETMASK_UNKNOWN)) // netmask is not needed for possible filters /// TODO - get error message on PCAP_ERROR
    {
        fprintf(stderr, "# Failed to compile packet catching filter for interface %s.\n", progpar.interface);
        pcap_close(opened_interface);
        return(2);
    }
    if (pcap_setfilter(opened_interface, &filter_program))
    {
        fprintf(stderr, "# Failed to set packet catching filter for interface %s.\n", progpar.interface);
        pcap_close(opened_interface);
        return(2);
    }
    pcap_freecode(&filter_program);

    // start catching packets
    if (pcap_loop(opened_interface, (int)progpar.number, packet_handler, NULL)) // loop didn't catch the specified number of packets, but ended
    {
        fprintf(stderr, "# Error occurred while catching packets.\n");
        pcap_close(opened_interface);
        return(2);
    }

    // end the program successfully
    pcap_close(opened_interface);
    // fprintf(stderr, "$ Successful ending.\n"); /// DEBUG - ending marker
    return 0;
}
