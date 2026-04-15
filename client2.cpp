#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    std::cout<<"client"<<std::endl;
    std::cout<<"client"<<std::endl;
    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(3000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    // sending connection request
    connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    // sending data
    long message = 134345345;
    send(clientSocket, &message, sizeof(message), 0);
    // closing socket
    close(clientSocket);
    return 0;
}
