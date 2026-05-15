#include "fileinfo.h"
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char fileDir[256];

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void readFileBytes(int clientSocket, const char filePath[256], uintmax_t fileSize) {
    if (FILE *fp = fopen(filePath, "wb")) {
        size_t readBytes;
        char buffer[8192];
        std::cout <<"fileSize: "<< fileSize << std::endl;
        int s = 0;
        while ((readBytes = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            if (fwrite(buffer, 1, readBytes, fp) != readBytes) {
                std::cout << "error al escribir" << std::endl;
                break;
            }
            s += readBytes;
            fileSize -= readBytes;
            //std::cout << "Received " << readBytes << " bytes" << std::endl;
            //std::cout << "file size subtracted " << fileSize << std::endl;
        }
        fclose(fp);
        std::cout << "bytes recibdos " << s << std::endl;
    }
    if (fileSize > 0) {
        std::cout << "error al recibir el archivo,archivo incompleto" << std::endl;
    } else
        std::cout << "File transfer complete." << std::endl;
}

void sendFileBytes(int clientSocket, const char filePath[256]) {
    if (FILE *fp = fopen(filePath, "rb")) {
        size_t readBytes;
        char buffer[8192];
        int s = 0;
        while ((readBytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            if (send(clientSocket, buffer, readBytes, 0) != readBytes) {
                perror("");
                //handleErrors();
                break;
            }
            s+=readBytes;
        }
        std::cout<<"bytes enviados: "<<s<<std::endl;
        fclose(fp);
    }
}

void buildFileDir() {
    struct passwd *pw = getpwuid(getuid());
    strncpy(fileDir, pw->pw_dir, strlen(pw->pw_dir));
    strncat(fileDir, "/sktFiles/", strlen("/sktFiles/"));
    std::cout << "fileDir: " << fileDir << std::endl;
    struct stat st = {0};
    mkdir(fileDir, 0777);
    if (stat(fileDir, &st) == -1)
        error("error al crear");
}

void receiveFile(int clientSocket){
    FileInfo fileInfo;
    if (recv(clientSocket, &fileInfo, sizeof(fileInfo), 0) < 0)
        error("cannot read file info");
    strncat(fileDir, fileInfo.fileName, strlen(fileInfo.fileName));
    readFileBytes(clientSocket, fileDir, fileInfo.fileSize);
}

void sendFile(int clientSocket){
    char fileName[50],filePath[50];
    if (recv(clientSocket, fileName, sizeof(fileName), 0) < 0)
        error("cannot read file name");
    strncpy(filePath, fileDir, strlen(fileDir));
    strncat(filePath,fileName,strlen(fileName));
    sendFileBytes(clientSocket,filePath);
}

void *thread_proc(void *arg) {
    int clientSocket = *((int *) (&arg));
    char op;
    if (recv(clientSocket, &op, sizeof(op), 0) < 0)
        error("cannot read option");
    if(op == '1')
        receiveFile(clientSocket);
    else if(op == '2')
        sendFile(clientSocket);
    else
        std::cout<<"opcion invalida"<<std::endl;
    close(clientSocket);
    return nullptr;
}

int main() {
    std::cout << "server started" << std::endl;
    buildFileDir();
    int serverSocket;
    struct sockaddr_in serv_addr, cli_addr;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        error("ERROR opening socket");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(3000);
    if (bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    if (listen(serverSocket, 5) < 0)
        error("Cannot listen on socket!");
    socklen_t clilen = sizeof(cli_addr);
    int clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
    if (clientSocket < 0)
        error("ERROR on accept");
    pthread_t threadId;
    int result = pthread_create(&threadId, NULL, thread_proc, (void *) clientSocket);
    if (result != 0)
        error("Could not create thread");
    pthread_join(threadId, NULL);
    close(serverSocket);
    return 0;
}
