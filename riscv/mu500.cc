#include <iostream>
#include "devices.h"
#include "processor.h"
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int sock, sock_recv;
static struct sockaddr_in addr, addr_recv;
static unsigned char button[2] = {0};

mu500_t::mu500_t(std::vector<processor_t*>&) {
    // setup socket for sending
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(65007);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // setup socket for receiving
    sock_recv = socket(AF_INET, SOCK_DGRAM, 0);
    addr_recv.sin_family = AF_INET;
    addr_recv.sin_port = htons(65008);
    addr_recv.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    bind(sock_recv, (struct sockaddr *)&addr_recv, sizeof(addr_recv));
    u_long val = 1;
    ioctl(sock_recv, FIONBIO, &val); // make the socket non-blocking
}

void mu500_t::tick(void)
{
    int size;
    char buf[2 + 2 + 1 + 1 + 10];

    size = recv(sock_recv, buf, sizeof(buf), 0);

    if (size > 0) {
        buf[2 + 2 + 1] = '\0';
        unsigned int offset, data;
#ifdef DEBUG
        std::cerr << "mu500: received: " << buf << std::endl;
#endif /* DEBUG */
        sscanf(buf, "%2x%2x", &offset, &data);
        if (0x48 <= offset && offset <= 0x49) {
            button[offset-0x48] = (unsigned char)data;
#ifdef DEBUG
            std::cerr << "mu500: wrote: " << offset-0x48 << ":" << data << std::endl;
#endif /* DEBUG */
        }
    }
}

bool mu500_t::load(reg_t offset, size_t len, uint8_t *bytes)
{
    if (0x48 <= offset && offset <= 0x49) {
        *bytes = button[offset-0x48];
        return true;
    }else{
        return false;
    }
}

bool mu500_t::store(reg_t offset, size_t len, const uint8_t* bytes) {
    char content[2+2+1+1];
    if (0x0 <= offset && offset <= 0x47) {
        snprintf(content, sizeof(content), "%2x%2x;", (unsigned int)offset, *bytes);
#ifdef DEBUG
        std::cerr << "mu500: " << content << std::endl;
#endif  /* DEBUG */
        sendto(sock, content, 5, 0, (struct sockaddr *)&addr, sizeof(addr));
        return true;
    }else{
        return false;
    }
}


