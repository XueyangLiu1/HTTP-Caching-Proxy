#ifndef __SERVER_H__
#define __SERVER_H__
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <cstdlib>
#include "response.hpp"

#include "assert.h"

using namespace std;

class Socket{
public:
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname;
    const char *port;

    Socket():status(0),socket_fd(0),host_info_list(NULL),hostname(NULL),port(NULL){}
    Socket(const char *port):status(0),socket_fd(0),host_info_list(NULL),hostname(NULL),port(port){}
    Socket(const char *hostname, const char *port):status(0),socket_fd(0),host_info_list(NULL),hostname(hostname),port(port){}
    ~Socket(){
        if(socket_fd!=0){
            close(socket_fd);
        }
        if(host_info_list!=NULL){
            free(host_info_list);
        }
    }
    void printInfo();
    std::string getHostPortInfo();
};

class Server : public Socket{
public:
    Server(const char *port):Socket(port){
        startAsServer();
    }
    void startAsServer();
    int accept_connection(string *ip_addr);//string *ip_addr as parameter, temporarily commented
};

class Client : public Socket{
public:
    Client(const char *hostname, const char *port):Socket(hostname,port){
        startAsClient();
    }
    void my_send(vector<char> &v);
    void my_recv(vector<char> &v);
    void startAsClient();
};

void my_recvFrom(int fd, vector<char> &v);
void my_sendTo(int fd, vector<char> &v);
void init_fdset(fd_set &readfds, vector<int> fds, int &nfds);
void checkMsgLen(int msg_len);
void sendString(int socket,string message);
string recvWithLen(int sender_fd,string message,int content_len);
string recvChunked(int sender_fd,string message);
#endif

