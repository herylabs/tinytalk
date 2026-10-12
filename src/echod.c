#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 7000

int main(){
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd < 0){
        perror("socket");
        return 1;
    }

    int on = 1;
    if(setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))){
        perror("setsocketopt");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if(bind(lfd, (void *)&addr, sizeof(addr)) < 0){
        perror("bind");
        return 1;
    }

    if(listen(lfd, 16) < 0){
        perror("listen");
        return 1;
    }

    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("Listen on the port %d\n", PORT);

    for(;;){
        struct sockaddr_in peer;
        socklen_t plen = sizeof(peer);

        int cfd = accept(lfd, (void*)&peer, &plen);
        if(cfd < 0){
            if(errno == EINTR)
                continue;

            perror("accept");
            break;
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &peer.sin_addr, ip, sizeof(ip));
        printf("client %s:%d connected\n", ip, ntohs(peer.sin_port));

        char buf[1024];
        ssize_t n;
        while ((n = recv(cfd, buf, sizeof(buf), 0)) > 0)
        {
            if(send(cfd, buf, (size_t)n, 0) < 0)
                break;
        }

        printf("client %s:%d disconnected\n", ip, ntohs(peer.sin_port));
        close(cfd);
        
    }

    close(lfd);
    return 0;
}