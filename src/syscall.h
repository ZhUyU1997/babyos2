/*
 * guzhoudiaoke@126.com
 * 2017-10-30
 */

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "types.h"


enum {
    SYS_PRINT = 0,
    SYS_FORK,
    SYS_EXEC,
	SYS_MMAP,
	SYS_EXIT,
	SYS_WAIT,
    SYS_SLEEP,
    MAX_SYSCALL,
};


class syscall_t {
public:
    static void  do_syscall(trap_frame_t* frame);
    static int32 sys_print(trap_frame_t* frame);
    static int32 sys_fork(trap_frame_t* frame);
    static int32 sys_exec(trap_frame_t* frame);
    static int32 sys_mmap(trap_frame_t* frame);
    static int32 sys_exit(trap_frame_t* frame);
    static int32 sys_wait(trap_frame_t* frame);
    static int32 sys_sleep(trap_frame_t* frame);

private:
    static char s_print_buffer[1024];
    static int32 (*s_system_call_table[])(trap_frame_t* frame);
};

#endif
