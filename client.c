#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT 80
#define MAX_LINE 256

int main(int argc, char* argv[])
{
        FILE *fp;
        struct hostent *hp;
        struct sockaddr_in sin;
        char *host;
        char buf[MAX_LINE];
        int s;
        int len;

        if (argc==2)
        {
        host = argv[1];
        }
        else
        {
        fprintf(stderr, "usage: client host\n");
        exit(1);
        }

        /* translate host name into peer's IP address */
        hp = gethostbyname(host);
        if (!hp)
        {
        fprintf(stderr, "client: unknown host %s\n", host);
        exit(1);
        }

        /* build address data structure */
        bzero((char *)&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        bcopy(hp->h_addr, (char*) &sin.sin_addr, hp->h_length);
        sin.sin_port = htons(SERVER_PORT);

        /* active open */
        if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
        perror("client: socket");
        exit(1);
        }
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        {
        perror("client: connect");
        close(s);
        exit(1);
        }
       /* main loop: get and send lines of text */
        while (fgets(buf, sizeof(buf), stdin))
        {
        char request[MAX_LINE] = {0};
        sprintf(request, "GET /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: text/plain\r\n\r\n", buf, host);
        buf[MAX_LINE-1] = '\0';
        len = strlen(request) + 1;
        send(s, request, len, 0);
        char response[MAX_LINE] = {0};
        while(len = recv(s, response, sizeof(response), 0))
        {
        fputs(response, stdout);
        }
        }
}
