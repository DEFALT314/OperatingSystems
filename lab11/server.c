#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#define MAX_USR 16
#define MAX_NAME_LEN 32
#define MAX_EVENTS 16
#define MAX_MSG_LEN 128

typedef struct {
    char name[MAX_NAME_LEN];
    int active;
    int fd;
    time_t last_active;
    int sent_pong;
} user_t;
int find_empty_index(const user_t *clients) {
    for (int i =0; i < MAX_USR; i++) {
        if (clients[i].active == 0)
            return i;
    }

    return -1;
}
bool is_prefix(char *prefix, char *str) {
    return strncmp(prefix, str, strlen(prefix)) ==0;
}
int main(int argc, char *argv[]){
    user_t clients[MAX_USR];
    int port = atoi(argv[1]);
    for (int i = 0; i < MAX_USR; i++) {
        clients[i].active = 0;
        clients[i].fd = -1;
        clients[i].sent_pong = 0;
    }

    int fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockaddr = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(port)};
    bind(fdsocket, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    listen(fdsocket, 5);
    union epoll_data epoll_data = {.fd = fdsocket};
    struct epoll_event socket_event= {.events = EPOLLIN, .data =  epoll_data};

    int epoll_fd = epoll_create1(0);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fdsocket, &socket_event);
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS,5000);
        for (int i = 0; i<n; i++) {
            if (fdsocket == events[i].data.fd) {
                int fd = accept(fdsocket, NULL, NULL);
                int idx = find_empty_index(clients);
                if (idx ==-1) {
                    close(fd);
                }
                else {
                    char buff[1024];
                    ssize_t x = read(fd, buff, sizeof(buff));
                    if (x > 0) {
                        buff[x] = '\0';
                        printf("Nowy klient dołączył: %s\n", buff);
                        strcpy(clients[idx].name, buff);
                        clients[idx].active = 1;
                        clients[idx].last_active = time(NULL);
                        clients[idx].fd = fd;
                        clients[idx].sent_pong = 0;

                        union epoll_data epoll_data_client = {.fd = fd};
                        struct epoll_event client_event = {.events = EPOLLIN, .data =  epoll_data_client};
                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &client_event);
                    }
                    else {
                        close(fd);
                    }
                }

            }
            else {
                int fd = events[i].data.fd;
                int id_client = -1;
                for (int j = 0; j < MAX_USR; j++) {
                    if (clients[j].fd == fd) {
                        id_client = j;
                        break;
                    }
                }

                char buff[1024];
                ssize_t n_read = read(fd, buff, sizeof(buff));
                if (n_read <= 0) {
                    clients[id_client].active = 0;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    continue;
                }
                clients[id_client].last_active = time(NULL);
                clients[id_client].sent_pong = 0;
                buff[n_read] = '\0';
                if ( is_prefix("LIST", buff)) {
                    char all_users[1024];
                    all_users[0] = '\0';
                    for (int j = 0; j < MAX_USR; j++) {
                        if (clients[j].active) {
                            strcat(all_users, "- ");
                            strcat(all_users, clients[j].name);
                            strcat(all_users, "\n");
                        }
                    }
                    write(fd, all_users, strlen(all_users));
                }
                else if (is_prefix("2ALL", buff)) {
                    char msg[MAX_MSG_LEN];
                    char formatted[MAX_MSG_LEN];
                    sscanf(buff, "2ALL %[^\n]", msg);
                    time_t current_time;
                    struct tm * time_info;
                    char timeString[9];
                    time(&current_time);
                    time_info = localtime(&current_time);
                    strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
                    snprintf(formatted, sizeof(formatted),"[%s] %s: %s\n",
                        timeString, clients[id_client].name, msg);
                    for ( int j = 0; j < MAX_USR; j++) {
                        if (clients[j].active && j != id_client) {
                            write(clients[j].fd, formatted, strlen(formatted));
                        }
                    }
                }
                else if ( is_prefix("2ONE", buff)) {
                    int id = -1;
                    char name[MAX_NAME_LEN];
                    char msg[MAX_MSG_LEN];
                    char formatted[MAX_MSG_LEN];
                    sscanf(buff, "2ONE %s %[^\n]",name, msg);
                    for (int j = 0; j < MAX_USR; j++) {
                        if (strcmp(clients[j].name, name) == 0) {
                            id = j;
                            break;
                        }
                    }
                    if (id != -1) {
                        time_t current_time;
                        struct tm * time_info;
                        char timeString[9];
                        time(&current_time);
                        time_info = localtime(&current_time);
                        strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
                        snprintf(formatted, sizeof(formatted),"[%s] %s: %s\n",
                            timeString, clients[id_client].name, msg);
                        write(clients[id].fd, formatted, strlen(formatted));
                    }
                }

                else if ( is_prefix("STOP", buff)) {
                    clients[id_client].active = 0;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
                else if ( is_prefix("ALIVE", buff)) {
                    clients[id_client].sent_pong = 0;
                    clients[id_client].last_active = time(NULL);

                }
            }

        }
        time_t current_time = time(NULL);
        for (int j = 0; j < MAX_USR; j++) {
            if (!clients[j].active)
                continue;
            if (clients[j].sent_pong==1 && (current_time - clients[j].last_active > 10)) {
                printf("Klient '%s' nie odpowiedział na PING. Rozłączanie.\n", clients[j].name);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clients[j].fd, NULL);
                close(clients[j].fd);
                clients[j].active = 0;

                continue;
            }
            if (clients[j].sent_pong==0 && (current_time - clients[j].last_active > 15)) {
                printf("Wysyłam PING do '%s'\n", clients[j].name);
                write(clients[j].fd, "PING", 4);
                clients[j].sent_pong = 1;
            }
        }
    }

}
