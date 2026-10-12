#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


/**
 * recv: Wait action from server (action)
 * accept: Wait action from client (connection, action)
 */

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage : %s <hôte> <port>\n", argv[0]);
        return 2;
    }
    
    /* 1. Résolution : nom + service -> liste d'adresses candidates */
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /* IPv4 ou IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    
    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        return 1;
    }
        
    /* 2. On essaie chaque adresse jusqu'à ce qu'une connexion réussisse */
    int fd = -1;
    for (p = res; p; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0)
           continue;

        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0)
            break;

        close(fd);
        fd = -1;
    }
    
    freeaddrinfo(res);
    if (fd < 0) {
        perror("connect");
        return 1;
    }

    /* 3. Requête HTTP/1.0 minimale */
    char req[512];
    int len = snprintf(req, sizeof req, "GET / HTTP/1.0\r\nHost: %s\r\nConnection:close\r\n\r\n", argv[1]);
    int sent = 0;
    while (sent < len) { /* send peut n'envoyer qu'une partie
        */
        ssize_t n = send(fd, req + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR)
            continue;
            perror("send");
            return 1;
        }
        sent += (int)n;
    }
    
    /* 4. Lecture de la réponse jusqu'à la fermeture par le serveur */
    char buf[4096];
    ssize_t n;
    while ((n = recv(fd, buf, sizeof buf, 0)) > 0)
        fwrite(buf, 1, (size_t)n, stdout);
    
    if (n < 0)
        perror("recv");
    
    close(fd);
    return 0;
}