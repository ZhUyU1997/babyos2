/*
 * guzhoudiaoke@126.com
 * 2017-12-10
 */

#ifndef _USERLIB_H_
#define _USERLIB_H_

#include "types.h"
#include "syscall.h"
#include "signal.h"
#include "fs.h"
#include "socket.h"
#include "sys_socket.h"
#include "screen.h"

#define PROT_NONE           0x0       /* page can not be accessed */
#define PROT_READ           0x1       /* page can be read */
#define PROT_WRITE          0x2       /* page can be written */
#define PROT_EXEC           0x4       /* page can be executed */


#define BUFFER_SIZE     1024

#define  _AUPBND         (sizeof (uint32) - 1)
#define  _ADNBND         (sizeof (uint32) - 1)
typedef char* va_list;
#define _bnd(X, bnd)    (((sizeof (X)) + (bnd)) & (~(bnd)))
#define va_arg(ap, T)   (*(T *)(((ap) += (_bnd (T, _AUPBND))) - (_bnd (T,_ADNBND))))
#define va_end(ap)      (void) 0
#define va_start(ap, A) (void) ((ap) = (((char *) &(A)) + (_bnd (A,_AUPBND))))

#define CHARACTER(ch)       (ch & 0xff)

class userlib_t {
    static const int fd_stdin  = 0;
    static const int fd_stdout = 1;
    static const int fd_error  = 2;

public:
    static int fork();
    static int exec(const char* path, argument_t* arg);
    static void *mmap(uint32 addr, uint32 len, uint32 prot, uint32 flags);
    static void exit(int val);
    static void wait(uint32 pid);
    static void kill(uint32 pid, uint32 sig);
    static void signal(uint32 sig, sighandler_t handler);

    static int  vsprintf(char *buffer, const char *fmt, va_list args);
    static int  sprintf(char* buffer, const char* fmt, ...);
	static int  printf(const char* fmt, ...);
    static void gets(char* buf, uint32 max);
    static void puts(char* buf);
    static int  color_print(color_ref_t color, const char *str);

    static void loop_delay(int32 loop);
    static void sleep(uint32 second);

    static char* strrev(char* str, int len);
    static void* memset(void* dst, uint32 c, uint32 n);
    static int   strlen(const char* str);
    static char* strcpy(char* dst, const char* src);
    static char* strncpy(char* dst, const char* src, int n);
    static int   strcmp(const char* s1, const char *s2);
    static int   strncmp(const char* s1, const char *s2, int n);
    static char* strcat(char* dst, const char* src);

    static int  open(const char* path, int mode);
    static int  close(int fd);
    static int  read(int fd, void* buf, uint32 size);
    static int  write(int fd, void* buf, uint32 size);
    static int  mkdir(const char* path);
    static int  link(const char* path_old, const char* path_new);
    static int  unlink(const char* path);
    static int  mknod(const char* path, int major, int minor);
    static int  dup(int fd);
    static int  chdir(const char* path);
    static int  fstat(int fd, stat_t* st);
    static int  stat(const char* path, stat_t* st);
    static int  pipe(int fd[2]);

    /* socket */
    static int  socket(int domain, int type, int protocol);
    static int  bind(int sockfd, const sock_addr_t* addr);
    static int  listen(int sockfd, int backlog);
    static int  connect(int sockfd, const sock_addr_t* addr);
    static int  accept(int sockfd, sock_addr_t* addr);

private:
    static int  sprint_int(char* buffer, int n, int width, int base, bool sign);
    static int  sprint_str(char* buffer, char* s, int width);
    static bool is_digit(char c);
};

#endif
