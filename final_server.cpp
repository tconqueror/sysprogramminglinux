#include <iostream>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <type_traits>
#include <unistd.h>
#include <thread>
#include <vector>
#include <string.h>
using namespace std;

// Checks If a c-style string is an integer
bool is_int(char *c)
{
    while (*c != '\0')
    {
        if (*c < '0' || *c > '9')
            return false;
        c++;
    }
    return true;
}
int sock_listener;
sockaddr_in server_addr;
void startListen(char *a, char * b);

int main(int argc, char **argv)
{
    if (argc != 3 || !is_int(argv[2]))
    {
        cout << "[ERROR] Port is not provided via command line parameters!\n";
        return -1;
    }
    thread listenThr(startListen,argv[1], argv[2]);
    listenThr.detach();
    string a;
    while (1)
    {
        cin >>a;
        if (!a.compare("Stop"))
            break;
    }
    return 0;
}
vector <int> listSocket;
void client(int sock_client, char* ip, char* port)
{
    char buf[1000]={0};
    string id = "[" + (string)ip + ":" + port + "] ";
    while (recv(sock_client, buf, 1000, 0) >0)
    {
        string formMsg = id + buf;
        for (auto i: listSocket)
        {
            if (i == sock_client) 
                continue;
            send(i, formMsg.c_str(), formMsg.size(), 0);
        }
    }
    for (vector<int>::iterator it = listSocket.begin();it!=listSocket.end();it++)
    {
        if (*it == sock_client)
        {
            listSocket.erase(it);
            break;
        }
    }
    close(sock_client);
    std::cout << "[INFO] Client " << ip << ":" << port << " is closed.\n";
}
void startListen(char *a, char * b)
{
    // Create a socket & get the file descriptor
    sock_listener = socket(AF_INET, SOCK_STREAM, 0);
    // Check If the socket is created
    if (sock_listener < 0)
    {
        cout << "[ERROR] Socket cannot be created!\n";
        return;
    }
    // Address info to bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::atoi(b));
    inet_pton(AF_INET, a, &server_addr.sin_addr);
    char buf[INET_ADDRSTRLEN];

    // Bind socket
    if (bind(sock_listener, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cout << "[ERROR] Created socket cannot be binded to ( "
                  << inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN)
                  << ":" << ntohs(server_addr.sin_port) << ")\n";
        return ;
    }

    std::cout << "[INFO] Sock is binded to ("
              << inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN)
              << ":" << ntohs(server_addr.sin_port) << ")\n";

    // Start listening
    if (listen(sock_listener, SOMAXCONN) < 0)
    {
        cout << "[ERROR] Socket cannot be switched to listen mode!\n";
        return ;
    }
    std::cout << "[INFO] Socket is listening now.\n";
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int sock_client;
    while ((sock_client = accept(sock_listener, (sockaddr *)&client_addr, &client_addr_size)) >= 0)
    {
        std::cout << "[INFO] A connection is accepted now.\n";

        // Get name info
        char host[NI_MAXHOST];
        char svc[NI_MAXSERV];
        if (getnameinfo((sockaddr *)&client_addr, client_addr_size,host, NI_MAXHOST, svc, NI_MAXSERV, 0) != 0)
        {
            std::cout << "[INFO] Client: (" << inet_ntop(AF_INET, &client_addr.sin_addr, buf, INET_ADDRSTRLEN)
                    << ":" << ntohs(client_addr.sin_port) << ")\n";
        }
        else
        {
            std::cout << "[INFO] Client: (host: " << host << ", service: " << svc << ") connected\n";
        }
        listSocket.push_back(sock_client);
        thread temp(client, sock_client, host, svc);
        temp.detach();
        // Close client socket
        
    }
}
