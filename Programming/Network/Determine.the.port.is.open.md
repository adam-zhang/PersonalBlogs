

``````
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 80

int main(void) {
    // C语言编程判断某端口是否被占用
    printf("port has been used:\n");

    int i;
    for (i = 1; i < 65536; i++) {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(i);
        inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
        if (bind(fd, (struct sockaddr *) (&addr), sizeof(sockaddr_in)) < 0) {
            printf("%d,", i);
        }
        close(fd);
    }

    printf("\n");
    return 0;
}
``````
