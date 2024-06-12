/* sdctool
   Copyright (c) 2024 bellrise */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static void usage()
{
    printf("usage: sdctool [-hv] FILE\n\n"
           "Create or decode SDC blobs.\n\n"
           "Options:\n"
           "  -c, --create      create based on description in FILE\n"
           "  -d, --decode      decode a SDC blob from FILE (default mode)\n"
           "  -h, --help        show this page and exit\n"
           "  -v, --version     show the version and exit\n");
}

static void version()
{
    puts("sdctool 0.1");
}

enum mode
{
    MODE_CREATE,
    MODE_DECODE
};

struct settings
{
    const char *file;
};

static int parse_opts(int argc, char **argv, struct settings *settings)
{
    static struct option opts[] = {{"create", no_argument, 0, 'c'},
                                   {"decode", no_argument, 0, 'd'},
                                   {"help", no_argument, 0, 'h'},
                                   {"version", no_argument, 0, 'v'},
                                   {NULL, 0, 0, 0}};
    int longind;
    int mode;
    int c;

    mode = MODE_DECODE;
    settings->file = NULL;

    while (1) {
        c = getopt_long(argc, argv, "cdhv", opts, &longind);
        if (c == -1)
            break;

        switch (c) {
        case 'c':
            mode = MODE_CREATE;
            break;
        case 'd':
            mode = MODE_DECODE;
            break;
        case 'h':
            usage();
            exit(0);
        case 'v':
            version();
            exit(0);
        }
    }

    if (optind < argc)
        settings->file = argv[optind++];

    return mode;
}

void create(const char *file);
void decode(const char *file);

int main(int argc, char **argv)
{
    struct settings settings;

    parse_opts(argc, argv, &settings) == MODE_CREATE ? create(settings.file)
                                                     : decode(settings.file);
}
