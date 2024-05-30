/* Header describing `sdc` binary data format.
   Copyright (c) 2023-2024 bellrise */

#ifndef _SDC_H
#define _SDC_H 1

#define SDC_MAGIC   "SDC"
#define SDC_VERSION 0x10

/* If your platform doesn't define uint8_t or uint16_t, please define
   __sdc_defined_integers and all sdc_uN types listed here, which are
   all unsigned integers N bits wide. */
#ifndef _sdc_defined_integers
# include <stdint.h> /* uint8_t, uint16_t, uint32_t */
typedef uint8_t sdc_u8;
typedef uint16_t sdc_u16;
typedef uint32_t sdc_u32;
# define _sdc_defined_integers 1
#endif

/* Header flags. */
#define SDC_FLE 0x00 /* numbers are little-endian */
#define SDC_FBE 0x01 /* numbers are big-endian (network) */

/* Header flags (format extensions). */
#define SDC_FEXTCOMPACT 0x01 /* compact entry extension */

/* Entry flags. */
#define SDC_ENAMED  0x01 /* named entry */
#define SDC_ESIZE32 0x02 /* entry32 is used instead */

/* Entry types. */
#define SDC_NULL   0 /* no type */
#define SDC_INT    1 /* 32 bit signed integer */
#define SDC_LONG   2 /* 64 bit signed integer */
#define SDC_UINT   3 /* 32 bit unsigned integer */
#define SDC_ULONG  4 /* 64 bit unsigned integer */
#define SDC_BOOL   5 /* boolean value */
#define SDC_STRING 6 /* string */
#define SDC_ARRAY  7 /* array */
#define SDC_BYTES  8 /* raw bytes */

/* The file format is as follows:

    +---+------------+----------------------------------------------------+
    |Off| Structure  | Description                                        |
    +---+------------+----------------------------------------------------+
    | 0 | sdc_header | File/stream header, contains flags and entry count |
    | 8 | sdc_entryN | First entry.                                       |
    | - |            | Data for the first entry.                          |
    | - | sdc_entryN | Second entry.                                      |
    | - |            | Data for the second entry.                         |
    | - |            | ...                                                |

*/

/* Header, placed at the start of the file. Contains the magic bytes
   (SDC_MAGIC), the format version (SDC_VERSION) and the flags. */
struct sdc_header
{
    sdc_u8 h_magic[3];   /* magic bytes */
    sdc_u8 h_version;    /* format version */
    sdc_u8 h_flags;      /* some storage for flags */
    sdc_u8 h_extflags;   /* extension flags */
    sdc_u16 h_userflags; /* space for user-defined flags */
    sdc_u16 h_entries;   /* number of top-level entries */
};

/* Two-byte aligned structure, representing a single entry. Declares the size of
   the data placed right after this entry. Note that if the entry is an array
   (e_type = SDC_ARRAY), then e_size contains the amount of elements. */
struct sdc_entry
{
    sdc_u8 e_type;  /* datatype stored */
    sdc_u8 e_flags; /* entry flags */
    sdc_u16 e_size; /* size of data, or element count for array */
};

/* Two-byte aligned structure, representing a single entry. When the basic
   sdc_entry has to SDC_ESIZE32 flag, an additional u16 is placed after the
   structure, to offer a wider size, which is calculated as:
   e_size = (e_sizelow | (e_sizehigh << 16)). */
struct sdc_entry32
{
    sdc_u8 e_type;      /* datatype stored */
    sdc_u8 e_flags;     /* entry flags */
    sdc_u16 e_sizelow;  /* size of data */
    sdc_u16 e_sizehigh; /* size of data << 16 */
};

#endif /* _SDC_H */
