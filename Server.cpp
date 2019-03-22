#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include "Server.h"

#define SV_SOCK_PATH "/tmp/unix.sock"

#define BACKLOG 5
#define BUF_SIZE 100

using namespace std;

void errExit(char* format, ...)
{
    va_list start;
    va_start(start, format);
    vprintf(format, start);
    exit(1);
}

void CServer::start()
{
    printf("CServer::start()\n");
    init();
    m_running = true;
    m_thread = new thread([this](){
        loop();
    });
}

void CServer::stop()
{
    m_running = false;
    m_thread->join();
}

void CServer::setOnMessage(void (*fn)(char* msg, int length))
{
    m_fn = fn;
}

void CServer::init()
{
    struct sockaddr_un addr;

    m_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_sock == -1)
        errExit("socket");

    /* Construct server socket address, bind socket to it,
       and make this a listening socket */

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        errExit("remove-%s", SV_SOCK_PATH);

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(m_sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1)
        errExit("bind");

    if (listen(m_sock, BACKLOG) == -1)
        errExit("listen");

    printf("unix domain socket init complete!!\n");
}

void CServer::loop()
{
    int cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];
    
    while (m_running) {          /* Handle client connections iteratively */

        /* Accept a connection. The connection is returned on a new
           socket, 'cfd'; the listening socket ('sfd') remains open
           and can be used to accept further connections. */

        cfd = accept(m_sock, NULL, NULL);
        if (cfd == -1)
            errExit("accept");

        /* Transfer data from connected socket to stdout until EOF */

        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
            (*m_fn)(buf, numRead);


        if (numRead == -1)
            errExit("read");

        if (close(cfd) == -1)
            printf("close");
    }
}

