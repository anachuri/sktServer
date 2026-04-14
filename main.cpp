#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(){
    int serverSocket;
    struct sockaddr_in serv_addr, cli_addr;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(3000);
    if (bind(serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(serverSocket, 5);
    socklen_t clilen = sizeof(cli_addr);
    int clientSocket = accept(serverSocket,(struct sockaddr *)&cli_addr,&clilen);
    if (clientSocket < 0)
        error("ERROR on accept");
    long fileSize;
    if(recv(clientSocket, &fileSize, sizeof(long), 0)<0)
        error("cannot read file size");
    std::cout<<fileSize<<std::endl;
    if (FILE *fp = fopen("image.jpeg", "wb")){
        size_t readBytes;
        char buffer[4096];
        while ((readBytes = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0 && fileSize>0){
            if (fwrite(buffer, 1, readBytes, fp) != readBytes){
                std::cout<<"error leyendo";
                break;
            }
            fileSize-=readBytes;
            std::cout << "Received " << readBytes << " bytes" << std::endl;
        }
        std::cout << "File transfer complete." << std::endl;
        fclose(fp);
    }
    close(clientSocket);
    close(serverSocket);
    return 0;
}
