#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BUFFER_SIZE 256
#define BACKLOG 10


void handle_request(int client_fd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_fd, (struct sockaddr*)&client_addr, &addr_len);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

    char buffer[MAX_BUFFER_SIZE] = {0};
    ssize_t bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0) {
        perror("recv");
        close(client_fd);
        return;
    }

    char *filename = buffer + 5;
    *strchr(filename, ' ') = 0;

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char timeStr[80];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);


    printf("[+] %s:%d %s /%s [%s]\n", client_ip, ntohs(client_addr.sin_port), "GET", filename, timeStr);

    int opened_fd = open(filename, O_RDONLY);
    if (opened_fd == -1) {
        perror("open");
        close(client_fd);
        return;
    }

    sendfile(client_fd, opened_fd, 0, bytesRead);

    close(opened_fd);
}


int main(int argc, char *argv[]) {
    int c;
    int port = htons(8080);

    while ((c = getopt(argc, argv, "p:")) != -1) {
        switch (c) {
            case 'p':
                int argPort = atoi(optarg);
                if (argPort > 65535 || argPort < 0) {
                    printf("Port number must be between 0 and 65535\n");
                    return -1;
                }
                unsigned short networkOrderPort = htons(argPort);
                port = networkOrderPort;
                break;

            default:
                break;
        }
    }

    int s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        AF_INET,
        port,
        0
    };
    printf("Using port: %d\n", ntohs(port));
    int bind_fd = bind(s, (struct sockaddr *)&addr, sizeof(addr));

    if (bind_fd != 0) {
        perror("bind");
        return 1;
    }
    listen(s, 10);

    while (1) {
        int client_fd = accept(s, NULL, NULL);
        
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        handle_request(client_fd);

        close(client_fd);
    }

    return 0;
}
