#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "parse.h"

static const char* DEFAULT_HOST = "0";
static const char* DEFAULT_PORT = "56789";

struct params
{
    const char* host_addr;
    const char* port_nr;
};

struct params* alloc_params(void)
{
    struct params* params;
    params = malloc(sizeof(struct params));
    if (params != 0) {
        memset(params, 0, sizeof *params);
        params->host_addr = DEFAULT_HOST;
        params->port_nr = DEFAULT_PORT;
    }
    return params;
}

int extract_params_from_cmdline_options(struct params* params, int argc, char* argv[])
{
    int rc = 0;

    if (!params) {
        rc = -1;
        return rc;
    }
    int optc;
    static struct option long_options[] = {
            {.val='a', .name="host-name", .has_arg=required_argument, .flag=0},
            {.val='a', .name="ip-address", .has_arg=required_argument, .flag=0},
            {.val='p', .name="port-number", .has_arg=required_argument, .flag=0},
            {0,0,0,0}
    };
    while(-1 != (optc = getopt_long(argc, argv, "a:p:h:", long_options, 0))) {
        if ('?' == optc) {
            optc = optopt;
        }
        update_params_with_opt(params, optc, optarg);
    }
    return rc;
}

int update_params_with_opt(struct params* params, int opt, const char* arg)
{
    int rc = 0;
    if (!params) {
        rc = -1;
        return rc;
    }
    switch (opt) {
        case 'a':
            params->host_addr = arg;
            break;
        case 'p':
            params->port_nr = arg;
            break;
        default:
            break;
    }
    return rc;
}

void free_params(struct params* params)
{
    if (params != 0) {
        free(params);
    }
}
const char* hostname(struct params* params)
{
    const char* name = 0;
    if (params) {
        name = params->host_addr;
    }
    return name;
}
const char* portnumber(struct params* params)
{
    const char* nr = 0;
    if (params) {
        nr = params->port_nr;
    }
    return nr;
}

