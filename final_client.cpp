#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <mutex>

using namespace std;
mutex m;
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
void comunication(int sock)
{
    char buf[1000]= {0};
    while (recv(sock, buf, 1000, 0) > 0)
    {
        cout << buf;
        memset(buf,0, sizeof(buf));
    }
    cout << "Server is disconnected" << endl;
    close(sock);
}
int main(int argc, char **argv)
{
    // Check arguments
    if (argc < 3 || !is_int(argv[2]))
    {
        cout << "[ERROR] Parameters are not valid!\n";
        return -1;
    }

    int port{std::atoi(argv[2])};
    // Address and host info
    sockaddr_in server_addr;
    hostent *server;

    // Create a socket & get the file descriptor
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Check If the socket is created
    if (sock_fd < 0)
    {
        cout << "[ERROR] Socket cannot be created!\n";
        return -2;
    }

    std::cout << "[INFO] Socket has been created.\n";

    // Get host information by name
    // gethostbyname is not thread-safe, checkout getaddrinfo
    server = gethostbyname(argv[1]);
    if (!server)
    {
        cout << "[ERROR] No such host!\n";
        return -3;
    }
    std::cout << "[INFO] Hostname is found.\n";

    // Fill address fields before try to connect
    std::memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Check if there is an address of the host
    if (server->h_addr_list[0])
        std::memcpy((char *)server->h_addr_list[0], (char *)&server_addr.sin_addr, server->h_length);
    else
    {
        cout << "[ERROR] There is no a valid address for that hostname!\n";
        return -5;
    }

    if (connect(sock_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cout << "Connection cannot be established!\n";
        return -6;
    }
    std::cout << "[INFO] Connection established.\n";
    thread pcomu(comunication, sock_fd);
    pcomu.detach();
    while (1)
    {
        string temp;
        getline(cin,temp);
        if (!temp.compare("exit"))
            break;
        temp = temp + "\n";
        //cout << temp << endl;
        //cout << temp.c_str() << endl;
        int a= send(sock_fd, temp.c_str(), temp.size()+1, 0);
    }
    return 0;
}