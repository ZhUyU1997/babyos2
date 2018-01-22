/*
 * guzhoudiaoke@126.com
 * 2018-01-20
 */

#ifndef _SOCKET_LOCAL_H_
#define _SOCKET_LOCAL_H_

#include "types.h"
#include "socket.h"
#include "spinlock.h"

#define MAX_LOCAL_SOCKET    128
#define MAX_LOCAL_PATH      108


class sock_addr_local_t : public sock_addr_t {
public:
    bool operator == (const sock_addr_local_t& addr);

public:
    char    m_path[MAX_LOCAL_PATH];
};

class socket_local_t : public socket_t {
public:
    socket_local_t();
    void  init();

    int32 create(uint32 family, uint32 type, uint32 protocol);
    int32 get_name(sock_addr_t* addr);
    int32 release();
    int32 dup(socket_t* socket);

    int32 bind(sock_addr_t* myaddr);
    int32 listen(uint32 backlog);
    int32 accept(socket_t* server_socket);
    int32 connect(sock_addr_t* user_addr);
    int32 read(void* buf, uint32 size);
    int32 write(void* buf, uint32 size);


    static void             init_local_sockets();
    static socket_t*        alloc_local_socket();
    static void             release_local_socket(socket_t* socket);
    static socket_local_t*  look_up_local_socket(sock_addr_local_t* addr);
    static int32            bind_local_socket(socket_local_t* socket, sock_addr_local_t* addr);

public:
    uint32              m_ref;
    sock_addr_local_t   m_addr;
    sock_buffer_t       m_sock_buf;

    static spinlock_t     s_lock;
    static socket_local_t s_local_sockets[MAX_LOCAL_SOCKET];
};

#endif