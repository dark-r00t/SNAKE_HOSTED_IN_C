#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<fcntl.h>

#define MAX_CONNECTIONS 1000
#define BYTES 1024

char *ROOT;
int listener, clients[MAX_CONNECTIONS];

void error(char *);

void startServer(char *);

void respond(int);

int
main(int argc, char *argv[]) {

    struct sockaddr_in addr_client;
    socklen_t addr_len;
    char c;

    char PORT[6];
    ROOT = getenv("PWD");
    strcpy(PORT, "8080");

    int slot = 0;

    while ((c = (char) getopt(argc, argv, "p:r:")) != -1) {
        switch (c) {
            case 'r':
                ROOT = malloc(strlen(optarg));
                strcpy(ROOT, optarg);
                break;
            case 'p':
                strcpy(PORT, optarg);
                break;
            default:
                exit(1);
        }
    }

    printf("Server started at port no. %s%s%s ", "\033[0;36m", PORT, "\033[0m");
    printf("with root directory as %s%s%s\n", "\033[0;36m", ROOT, "\033[0m");

    int i;

    for (i = 0; i < MAX_CONNECTIONS; i++) {
        clients[i] = -1;
    }

    startServer(PORT);

    while (1) {

        addr_len = sizeof(addr_client);
        clients[slot] = accept(listener, (struct sockaddr *) &addr_client, &addr_len);

        if (clients[slot] < 0) {
            error("accept() error");
        }

        if (fork() == 0) {
            respond(slot);
            exit(0);
        }

        while (clients[slot] != -1) {
            slot = (slot + 1) % MAX_CONNECTIONS;
        }
    }
}

void
startServer(char *port) {

    struct addrinfo cur, *res, *t;

    memset(&cur, 0, sizeof(cur));
    cur.ai_family = AF_INET;
    cur.ai_socktype = SOCK_STREAM;
    cur.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &cur, &res) != 0) {

        perror("Error getting address info.\n");
        exit(1);
    }

    t = res;
    while (t) {

        listener = socket(t->ai_family, t->ai_socktype, 0);

        if (bind(listener, t->ai_addr, t->ai_addrlen) == 0) {
            break;
        }

        t = t->ai_next;
    }

    if (t == NULL) {

        perror("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(listener, 1000000) != 0) {

        perror("listen() error");
        exit(1);
    }
}

void
respond(int n) {

    char output[99999], *request[3], input[BYTES], path[99999];
    ssize_t received, fd, bytes_read;

    memset((void *) output, (int) '\0', 99999);

    received = recv(clients[n], output, 99999, 0);

    if (received < 0) {

        fprintf(stderr, ("Error receiving information.\n"));
    }

    if (received == 0) {
        fprintf(stderr, "Client disconnected.\n");
    }

    if (received > 0){
        printf("%s", output);
        request[0] = strtok(output, " \t\n");
        if (strncmp(request[0], "GET\0", 4) == 0) {

            request[1] = strtok(NULL, " \t");
            request[2] = strtok(NULL, " \t\n");

            if (strncmp(request[2], "HTTP/1.0", 8) != 0 && strncmp(request[2], "HTTP/1.1", 8) != 0) {
                write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
            }

            else {
                if (strncmp(request[1], "/\0", 2) == 0) {
                    request[1] = "/index.html";
                }

                strcpy(path, ROOT);
                strcpy(&path[strlen(ROOT)], request[1]);
                printf("file: %s\n", path);

                if ((fd = open(path, O_RDONLY)) != -1) {
                    send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
                    while ((bytes_read = read((int) fd, input, BYTES)) > 0)
                        write(clients[n], input, bytes_read);
                } else {
                    write(clients[n], "HTTP/1.0 404 Not Found\n", 23);
                }
            }
        }
    }

    shutdown(clients[n], SHUT_RDWR);
    close(clients[n]);
    clients[n] = -1;
}
