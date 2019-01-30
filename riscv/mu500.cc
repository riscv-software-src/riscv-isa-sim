#include <iostream>
#include "devices.h"
#include "processor.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

static int sock;
static struct sockaddr_in addr;

mu500_t::mu500_t(std::vector<processor_t*>&) {
 sock = socket(AF_INET, SOCK_DGRAM, 0);
 addr.sin_family = AF_INET;
 addr.sin_port = htons(65007);
 addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

bool mu500_t::load(reg_t addr, size_t len, uint8_t* bytes) {
    return true;
}

bool mu500_t::store(reg_t offset, size_t len, const uint8_t* bytes) {
    char content[2+1+1+1];
    if (0x0 <= offset && offset <= 0x47) {
        snprintf(content, sizeof(content), "%2x%1x;", (unsigned int)offset, *bytes);
        std::cerr << "mu500: " << content << std::endl;
        sendto(sock, content, 4, 0, (struct sockaddr *)&addr, sizeof(addr));
        return true;
    }else{
        return false;
    }
}


