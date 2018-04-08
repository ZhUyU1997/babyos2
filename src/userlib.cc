/*
 * guzhoudiaoke@126.com
 * 2017-12-10
 */

#include "userlib.h"

static char digits[] = "0123456789abcdef";

int userlib_t::fork()
{
    int ret;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_FORK));
    return ret;
}

int userlib_t::exec(const char* path, argument_t* arg)
{
    int ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_EXEC), "b" (path), "c" (arg));
    return ret;
}

int userlib_t::color_print(color_ref_t color, const char *str)
{
    int ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_PRINT), "b" (color), "c" (str) );
    return ret;
}

void *userlib_t::mmap(uint32 addr, uint32 len, uint32 prot, uint32 flags)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_MMAP), "b" (addr), "c" (len), "d" (prot), "S" (flags));

    return (void*) ret;
}

void userlib_t::exit(int val)
{
    __asm__ volatile("int $0x80" : : "b" (val), "a" (SYS_EXIT));
}

void userlib_t::wait(uint32 pid)
{
    __asm__ volatile("int $0x80" : : "b" (pid), "a" (SYS_WAIT));
}

char* userlib_t::strrev(char* str, int len)
{
    char* s = str;
    char* e = str + len -1;
    while (s < e) {
        char c = *s;
        *s++ = *e;
        *e-- = c;
    }

    return str;
}

void userlib_t::loop_delay(int32 loop)
{
    while (--loop > 0) {
        __asm__("nop");
    }
}

void userlib_t::sleep(uint32 second)
{
    __asm__ volatile("int $0x80" : : "b" (second), "a" (SYS_SLEEP));
}

void userlib_t::kill(uint32 pid, uint32 sig)
{
    __asm__ volatile("int $0x80" : : "b" (pid), "c"(sig), "a" (SYS_KILL));
}

void userlib_t::signal(uint32 sig, sighandler_t handler)
{
    __asm__ volatile("int $0x80" : : "b" (sig), "c"(handler), "a" (SYS_SIGNAL));
}


// file
int userlib_t::open(const char* path, int mode)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_OPEN), "b" (path), "c" (mode));
    return ret;
}

int userlib_t::close(int fd)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_CLOSE), "b" (fd));
    return ret;
}

int userlib_t::read(int fd, void* buf, uint32 size)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_READ), "b" (fd), "c" ((uint32) buf), "d" (size));
    return ret;
}

int userlib_t::write(int fd, void* buf, uint32 size)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_WRITE), "b" (fd), "c" ((uint32) buf), "d" (size));
    return ret;
}

int userlib_t::mkdir(const char* path)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_MKDIR), "b" (path));
    return ret;
}

int userlib_t::link(const char* path_old, const char* path_new)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_LINK), "b" ((uint32) path_old), "c" ((uint32) path_new));
    return ret;
}

int userlib_t::unlink(const char* path)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_UNLINK), "b" (path));
    return ret;
}

int userlib_t::mknod(const char* path, int major, int minor)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_MKNOD), "b" (path), "c" (major), "d" (minor));
    return ret;
}

int userlib_t::dup(int fd)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_DUP), "b" (fd));
    return ret;
}

int userlib_t::chdir(const char* path)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_CHDIR), "b" (path));
    return ret;
}

int userlib_t::fstat(int fd, stat_t* st)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_STAT), "b" (fd), "c" (st));
    return ret;
}

int userlib_t::stat(const char* path, stat_t* st)
{
    int fd = open(path, file_t::MODE_RDONLY);
    if (fd < 0) {
        return -1;
    }

    int ret = fstat(fd, st);
    close(fd);
    return ret;
}

int userlib_t::pipe(int fd[2])
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_PIPE), "b" (fd));
    return ret;
}

int userlib_t::socket(int domain, int type, int protocol)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" 
            : "=a" (ret) 
            : "a" (SYS_SOCKET), "b" (sys_socket_t::SOCK_SOCKET), "c"(domain), "d" (type), "S" (protocol));
    return ret;
}

int userlib_t::bind(int sockfd, const sock_addr_t* addr)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" 
            : "=a" (ret) 
            : "a" (SYS_SOCKET), "b" (sys_socket_t::SOCK_BIND), "c"(sockfd), "d" (addr));
    return ret;
}

int userlib_t::listen(int sockfd, int backlog)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" 
            : "=a" (ret) 
            : "a" (SYS_SOCKET), "b" (sys_socket_t::SOCK_LISTEN), "c"(sockfd), "d" (backlog));
    return ret;
}

int userlib_t::connect(int sockfd, const sock_addr_t* addr)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" 
            : "=a" (ret) 
            : "a" (SYS_SOCKET), "b" (sys_socket_t::SOCK_CONNECT), "c"(sockfd), "d" (addr));
    return ret;
}

int userlib_t::accept(int sockfd, sock_addr_t* addr)
{
    uint32 ret = 0;
    __asm__ volatile("int $0x80" 
            : "=a" (ret) 
            : "a" (SYS_SOCKET), "b" (sys_socket_t::SOCK_ACCEPT), "c"(sockfd), "d" (addr));
    return ret;
}


/***********************************************************************/

void* userlib_t::memset(void *dst, uint32 c, uint32 n)
{
    char* d = (char *) dst;
    for (uint32 i = 0; i < n; i++) {
        *d++ = (c & 0xff);
    }

    return dst;
}

int userlib_t::strlen(const char* s)
{
    int len = 0;
    while (*s++) {
        len++;
    }
    return len;
}


char* userlib_t::strcpy(char* dst, const char* src)
{
	char* d = dst;
	while (*src) {
		*d++ = *src++;
	}
    *d++ = '\0';

	return dst;
}

char* userlib_t::strncpy(char* dst, const char* src, int n)
{
	char* d = dst;
	while (--n >= 0 && *src) {
		*d++ = *src++;
	}
    *d++ = '\0';

	return dst;
}

int userlib_t::strcmp(const char* s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }

    return *s1 - *s2;
}

int userlib_t::strncmp(const char* s1, const char *s2, int n)
{
    while (*s1 && *s2 && *s1 == *s2 && --n >= 0) {
        s1++;
        s2++;
    }

    return n == 0 ? 0 : *s1 - *s2;
}

char* userlib_t::strcat(char* dst, const char* src)
{
    char* ret = dst;
    while (*dst) {
        dst++;
    }
    while (*src) {
        *dst++ = *src++;
    }
    return ret;
}

int userlib_t::sprint_int(char* buffer, int n, int width, int base, bool sign)
{
    char buf[16] = {0};

    uint32 num = (uint32)n;
    if (sign && (sign = (n < 0))) {
        num = -n;
    }

    int i = 0;
    do {
        buf[i++] = digits[num % base];
        num /= base;
    } while (num != 0);

    if (sign) {
        buf[i++] = '-';
    }

    if (width < i) {
        width = i;
    }

    int len = i;
    while (--i >= 0) {
        *buffer++ = buf[i];
    }

    for (int i = 0; i < width - len; i++) {
        *buffer++ = ' ';
    }

    return width;
}

int userlib_t::sprint_str(char* buffer, char* s, int width)
{
    if (s == NULL) {
        return 0;
    }

    int len = strlen(s);
    if (width < len) {
        width = len;
    }

    strcat(buffer, s);
    buffer += len;
    for (int i = 0; i < width - len; i++) {
        *buffer++ = ' ';
    }
    return width;
}

bool userlib_t::is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

static char str_null[] = "NULL";
int userlib_t::vsprintf(char *buffer, const char *fmt, va_list ap)
{
    buffer[0] = '\0';
    if (fmt == NULL) {
        return 0;
    }

    int total = 0;
    //va_list ap = args;
    //va_start(ap, fmt);

    char c;
    int width = 0;
    char* s = NULL;
    for (int i = 0; (c = CHARACTER(fmt[i])) != 0; i++) {
        if (c != '%') {
            buffer[total++] = c;
            continue;
        }

        c = CHARACTER(fmt[++i]);
        if (c == '\0') {
            break;
        }

        width = 0;
        while (c != '\0' && is_digit(c)) {
            width = width * 10 + c - '0';
            c = CHARACTER(fmt[++i]);
        }

        if (c == '\0') {
            break;
        }

        switch (c) {
        case 'd':
            total += sprint_int(buffer + total, va_arg(ap, int32), width, 10, true);
            break;
        case 'u':
            total += sprint_int(buffer + total, va_arg(ap, int32), width, 10, false);
            break;
        case 'x':
        case 'p':
            total += sprint_int(buffer + total, va_arg(ap, int32), width, 16, false);
            break;
        case 'c':
            buffer[total++] = (char) CHARACTER(va_arg(ap, int32));
            break;
        case 's':
            total += sprint_str(buffer + total, va_arg(ap, char *), width);
            break;
        case '%':
            buffer[total++] = '%';
            break;
        default:
            buffer[total++] = '%';
            buffer[total++] = c;
            break;
        }
    }

    //va_end(ap);
    return total;
}

int userlib_t::sprintf(char* buffer, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int total = vsprintf(buffer, fmt, ap);
    va_end(ap);

    return total;
}

/* only support %d %u %x %p %c %s */
int userlib_t::printf(const char *fmt, ...)
{
    char buffer[BUFFER_SIZE] = {0};

    va_list ap;
    va_start(ap, fmt);
    int total = vsprintf(buffer, fmt, ap);
    va_end(ap);

    write(fd_stdout, buffer, total);
    return total;
}


void userlib_t::gets(char* buf, uint32 max)
{
    userlib_t::memset(buf, 0, max);
    int i = 0;
    while (i < max) {
        char c;
        int n = userlib_t::read(fd_stdin, &c, 1);
        if (n == 1) {
            if (c == '\n') {
                break;
            }
            *buf++ = c;
            i++;
        }
    }
}

void userlib_t::puts(char* buf)
{
    userlib_t::write(fd_stdout, buf, userlib_t::strlen(buf));
}


uint16 userlib_t::htons(uint16 n)
{
  return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
}

uint16 userlib_t::ntohs(uint16 n)
{
  return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
}

uint32 userlib_t::htonl(uint32 n)
{
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

uint32 userlib_t::ntohl(uint32 n)
{
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

uint32 userlib_t::make_ipaddr(uint8 ip0, uint8 ip1, uint8 ip2, uint8 ip3)
{
    return (uint32) ((uint32) ip0 << 24) | ((uint32) ip1 << 16) | ((uint32) ip2 << 8) | ip3;
}

uint16 userlib_t::check_sum(uint8* data, uint32 len)
{
    uint32 acc = 0;
    uint16* p = (uint16 *) data;

    int i = len;
    for (; i > 1; i -= 2, p++) {
        acc += *p;
    }

    if (i == 1) {
        acc += *((uint8 *) p);
    }

    while (acc >> 16) {
        acc = (acc & 0xffff) + (acc >> 16);
    }

    return (uint16) ~acc;
}

int userlib_t::send_to(int fd, void *buf, uint32 size, sock_addr_t* addr)
{
    if (buf == NULL || addr == NULL) {
        return -1;
    }

    uint32 ret = -1;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_SENDTO), "b" (fd), "c" ((uint32) buf), "d" (size), "S"(addr));
    return ret;
}

int userlib_t::recv_from(int fd, void *buf, uint32 size, sock_addr_t* addr)
{
    if (buf == NULL || addr == NULL) {
        return -1;
    }

    uint32 ret = -1;
    __asm__ volatile("int $0x80" : "=a" (ret) : "a" (SYS_RECVFROM), "b" (fd), "c" ((uint32) buf), "d" (size), "S"(addr));

    return ret;
}

void* userlib_t::memmov(void *dst, const void *src, uint32 n)
{
    const char *s = (const char *) src;
    char *d = (char *) dst;

    if (s < d && s + n > d) {
        s += n, d += n;
        while (n--) {
            *--d = *--s;
        }
    }
    else {
        while (n--) {
            *d++ = *s++;
        }
    }

    return dst;
}

void* userlib_t::memcpy(void *dst, const void *src, uint32 n)
{
    return memmov(dst, src, n);
}

