#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define MAX_USR 16
#define MAX_NAME_LEN 32
#define MAX_EVENTS 2
#define MAX_MSG_LEN 128
bool is_prefix(char *prefix, char *str) {
    return strncmp(prefix, str, strlen(prefix)) ==0;
}
int stop_flag = 0;
void handle_sigint(int sig) {
    stop_flag = 1;
}
int main(int argc, char *argv[]) {
    char* name = argv[1];
    char* ip = argv[2];
    char* port = argv[3];
    signal(SIGINT, handle_sigint);
    struct in_addr addr;
    inet_aton(ip, &addr);
    struct sockaddr_in sockaddr = {.sin_family = AF_INET, .sin_addr = addr, .sin_port = htons(atoi(port))};
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    connect(server_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    write(server_fd, name, strlen(name));
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    struct epoll_event events[MAX_EVENTS];
    struct epoll_event in_event = {.events = EPOLLIN, .data.fd = STDIN_FILENO};
    struct epoll_event socket_event = {.events = EPOLLIN, .data.fd = server_fd};
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &socket_event);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &in_event);
    char buffer[MAX_MSG_LEN];

    while (!stop_flag) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                fgets(buffer, MAX_MSG_LEN, stdin);
                write(server_fd, buffer, strlen(buffer));
                if (is_prefix("STOP", buffer)) {
                    stop_flag = 1;
                    break;
                }
            }
            else if (events[i].data.fd == server_fd) {
                ssize_t bytes_read = read(server_fd, buffer, MAX_MSG_LEN);
                if (bytes_read <= 0) {
                    printf("Serwer zamknął połączenie.\n");
                    stop_flag = 1;
                    break;
                }
                buffer[bytes_read] = '\0';
                if (is_prefix("PING", buffer)) {
                    write(server_fd, "ALIVE", 5);
                }
                else {
                    printf("%s", buffer);
                    fflush(stdout);
                }

            }
        }
    }
    printf("Diconnected\n");
    write(server_fd, "STOP", 4);
    close(server_fd);
    close(epoll_fd);
    return 0;


}
