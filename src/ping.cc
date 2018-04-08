/*
 * guzhoudiaoke@126.com
 * 2018-04-07
 */

#include "userlib.h"
#include "icmp.h"

const uint32 c_invalid_ip = 0xffffffff;
const uint32 c_packet_size = 32;

uint32 parse_ip(const char* str)
{
    uint32 ip = 0;
    if (str == NULL) {
        return c_invalid_ip;
    }
    userlib_t::printf("parse_ip: %s\n", str);

    const char* p = str;
    int count = 0;
    while (*p != '\0') {
        uint32 tmp = 0;
        while (*p != '\0' && *p != '.') {
            /* is numeric */
            if (*p < '0' || *p > '9') {
                return c_invalid_ip;
            }

            tmp = tmp*10 + *p - '0';
            if (tmp > 255) {
                return c_invalid_ip;
            }
            p++;
        }

        ip = ip << 8 | tmp;
        count++;
        p++;

        if (count > 4) {
            return c_invalid_ip;
        }
    }

    return ip;
}

void prepare_ping_data(uint8* data, uint32 len, uint16 seq)
{
    icmp_echo_hdr_t hdr;

    uint8  m_type;
    uint8  m_code;
    uint16 m_check_sum;
    uint16 m_id;
    uint16 m_seq_no;

    hdr.m_type = icmp_t::ECHO_REQUEST;      /* type */
    hdr.m_code = 0;                         /* code */ 
    hdr.m_check_sum = 0;                    /* check sum */
    hdr.m_id = 0;                           /* id */
    hdr.m_seq_no = userlib_t::htons(seq);   /* seq no */

    hdr.m_check_sum = userlib_t::check_sum((uint8 *) &hdr, sizeof(icmp_echo_hdr_t));
    userlib_t::memcpy(data, &hdr, sizeof(icmp_echo_hdr_t));
}

void ping(uint32 ip)
{
    int sock_fd = userlib_t::socket(socket_t::AF_INET, socket_t::SOCK_RAW, socket_t::PROTO_ICMP);
    if (sock_fd < 0) {
        userlib_t::printf("client create socket failed, error %u\n", sock_fd);
        return;
    }
    userlib_t::printf("client create socket success, fd: %u\n", sock_fd);

    sock_addr_inet_t dst_addr;
    dst_addr.m_family = socket_t::AF_INET;
    dst_addr.m_ip = ip;
    dst_addr.m_port = 0;

    uint32 size = c_packet_size + sizeof(icmp_echo_hdr_t);
    for (int i = 0; i < 4; i++) {
        uint8 data[size] = {0};
        prepare_ping_data(data, c_packet_size + sizeof(icmp_echo_hdr_t), i);
        
        if (userlib_t::send_to(sock_fd, data, size, &dst_addr) < 0) {
            userlib_t::printf("failed to send\n");
        }

        if (userlib_t::recv_from(sock_fd, data, size, &dst_addr) < 0) {
            userlib_t::printf("failed to receive\n");
        }

        userlib_t::sleep(1);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        userlib_t::printf("Usage: ping a.b.c.d\n");
        userlib_t::exit(0);
    }

    uint32 ip = parse_ip(argv[1]);
    if (ip == c_invalid_ip) {
        userlib_t::printf("invalid ip.\n");
        userlib_t::exit(0);
    }

    uint8* p = (uint8 *) &ip;
    userlib_t::printf("parse ip: %d.%d.%d.%d\n", p[3], p[2], p[1], p[0]);

    ping(ip);

    userlib_t::exit(0);
    return 0;
}

