/* sdc create
   Copyright (c) 2024 bellrise */

#include <assert.h>
#include <ctype.h>
#include <sdc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct entry
{
    sdc_u8 e_type;  /* datatype stored */
    sdc_u8 e_flags; /* entry flags */
    sdc_u16 size;   /* size of data, or element count for array */
    struct entry *next;
    struct entry *child;
    char *name;
    union
    {
        long ival;
        struct
        {
            char *strval;
            int strsiz;
        };
    };
};

struct info
{
    struct sdc_header header;
    struct entry *first;
    FILE *file;
};

struct entry *entry_new(int type, int flags)
{
    struct entry *ptr;

    ptr = malloc(sizeof(struct entry));

    ptr->e_type = type;
    ptr->e_flags = flags;
    ptr->size = 0;
    ptr->next = 0;
    ptr->child = 0;
    ptr->name = NULL;

    return ptr;
}

void put_child(struct entry *block, struct entry *child)
{
    block->size += 1;

    if (!block->child) {
        block->child = child;
        return;
    }

    block = block->child;
    while (block->next)
        block = block->next;

    block->next = child;
}

void die(const char *msg)
{
    printf("sdctool: error: %s\n", msg);
    exit(1);
}

char *strip(char *str)
{
    int siz;

    while (isspace(*str))
        str++;

    siz = strlen(str);
    while (isspace(str[--siz]))
        str[siz] = 0;

    return str;
}

void collect(struct info *info)
{
    struct entry *block;
    struct entry *entry;
    char line[1024];
    char *p;
    char *q;

    info->first = entry_new(SDC_NULL, 0xff);
    block = info->first;

    while (fgets(line, 1024, info->file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        p = strip(line);

        if (!strncmp(p, "INT", 3)) {
            entry = entry_new(SDC_INT, 0);
            put_child(block, entry);

            p += 4;
            if (*p == '(') {
                q = p + 1;
                while (*q != ')')
                    q++;

                entry->name = strndup(p + 1, q - p - 1);
                entry->e_flags |= SDC_ENAMED;
                entry->size = 4;
                p = q + 1;
            }

            entry->ival = strtol(p, NULL, 0);
        }
    }
}

void dump_entry(struct info *info, struct entry *entry)
{
    char zero = 0;
    int siz;

    fwrite(entry, 2, 1, stdout);
    fwrite(&entry->size, 2, 1, stdout);

    if (entry->e_flags & SDC_ENAMED) {
        siz = strlen(entry->name);
        if (siz > 254)
            die("names currently over 254 bytes are not supported");

        fwrite(&siz, 1, 1, stdout);
        fwrite(entry->name, siz, 1, stdout);

        if (siz % 2 == 0)
            fwrite(&zero, 1, 1, stdout);
    }

    switch (entry->e_type) {
    case SDC_INT:
        fwrite(&entry->ival, 4, 1, stdout);
        break;
    default:
        die("unimplemented e_type");
    }
}

void dump(struct info *info)
{
    struct entry *walker;

    info->header.h_entries = info->first->size;
    walker = info->first->child;

    fwrite(&info->header, sizeof(struct sdc_header), 1, stdout);

    while (walker) {
        dump_entry(info, walker);
        walker = walker->next;
    }
}

void create(const char *file)
{
    struct info info;

    static_assert(SDC_VERSION == 0x10, "this program is written for 1.0 spec");

    /* Setup header */

    memcpy(info.header.h_magic, SDC_MAGIC, 3);
    info.header.h_version = SDC_VERSION;
    info.header.h_flags = SDC_FLE;
    info.header.h_extflags = 0;
    info.header.h_userflags = 0;
    info.header.h_entries = 0;

    info.file = fopen(file, "r");
    if (!info.file)
        die("failed to open file");

    collect(&info);
    dump(&info);

    fclose(info.file);
}
