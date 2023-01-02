/* Header describing `sdc` binary data format.
   Copyright (c) 2023 bellrise */

#ifndef _SDC_H
#define _SDC_H 1


#define SDC_MAGIC       "SDC"
#define SDC_VERSION     0


/* If your platform doesn't define uint8_t or uint16_t, please define
   __sdc_defined_integers and all sdc_uN types listed here, which are
   all unsigned integers N bits wide. */
#ifndef __sdc_defined_integers
#include <stdint.h>     /* uint8_t, uint16_t */
typedef uint8_t  sdc_u8;
typedef uint16_t sdc_u16;
#define __sdc_defined_integers 1
#endif


#define SDC_ENAMED      0x01    /* named entry */


#define SCD_NULL        0       /* no type */
#define SDC_INT         1       /* 32 bit signed integer */
#define SDC_LONG        2       /* 64 bit signed integer */
#define SDC_UINT        3       /* 32 bit unsigned integer */
#define SDC_ULONG       4       /* 64 bit unsigned integer */
#define SDC_BOOL        5       /* boolean value */
#define SDC_STRING      6       /* string */
#define SDC_ARRAY       7       /* array */


struct sdc_header
{
	sdc_u8 h_magic[3];      /* magic bytes */
	sdc_u8 h_version;       /* format version */
	sdc_u8 h_flags;         /* some storage for flags */
	sdc_u8 h_customflags;   /* space for user-defined flags */
	sdc_u16 h_entries;      /* number of top-level entries */
};

struct sdc_entry
{
	sdc_u8 e_type;          /* datatype stored */
	sdc_u8 e_flags;         /* entry flags */
	sdc_u16 e_size;         /* size of data */
};

struct sdc_array
{
	sdc_u16 e_elems;        /* number of elements in array */
};


#endif /* _SDC_H */
