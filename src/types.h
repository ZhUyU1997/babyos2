/*
 * guzhoudiaoke@126.com
 * 2017-10-21
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#define NULL				(0)

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;
typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef int32				pid_t;

/* video info */
typedef struct video_info_s {
    uint16 video_mode;
    uint16 width;
    uint16 height;
    uint8  bits_per_pixel;
    uint8  memory_model;
    uint8* vram_base_addr;
} video_info_t;

typedef struct address_range_s {
	uint32	base_addr_low;
	uint32	base_addr_high;
	uint32	length_low;
	uint32	lenght_high;
    uint32  type;
} address_range_t;

typedef struct memory_layout_s {
	uint32 num_of_range;
	address_range_t ranges[32];
} memory_layout_t;

typedef struct rgb_s {
    uint8 r;
    uint8 g;
    uint8 b;
} rgb_t;

typedef struct rect_s {
    int32 left;
    int32 top;
    uint32 width;
    uint32 height;
} rect_t;

typedef uint32 color_ref_t;

typedef uint32  pde_t;
typedef uint32  pte_t;


typedef struct trap_frame_s {
    uint32 ebx;
    uint32 ecx;
    uint32 edx;
    uint32 esi;
    uint32 edi;
    uint32 ebp;
    uint32 eax;

    uint16 gs, padding4;
    uint16 fs, padding3;
    uint16 ds, padding1;
    uint16 es, padding2;

    uint32 trapno;

    // pushed by x86 hardware
    uint32 err;
    uint32 eip;
    uint16 cs;
    uint16 padding5;
    uint32 eflags;

    // cross rings
    uint32 esp;
    uint16 ss, padding6;
} trap_frame_t;

#define MAX_ARGS 16
#define MAX_ARG_LEN 32

typedef struct argument_s {
    unsigned m_argc;
    char m_argv[MAX_ARGS][MAX_ARG_LEN];
} argument_t;


#endif
