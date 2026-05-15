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

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void readFileBytes(int clientSocket, const char filePath[256], uintmax_t fileSize) {
    if (FILE *fp = fopen(filePath, "wb")) {
        size_t readBytes;
        char buffer[8192];
        std::cout << fileSize << std::endl;
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
        close(clientSocket);
    } else
        std::cout << "File transfer complete." << std::endl;
}

void buildFilePath(char *filePath, const char fileName[]) {
    struct passwd *pw = getpwuid(getuid());
    strncpy(filePath, pw->pw_dir, strlen(pw->pw_dir));
    //filePath[strlen(pw->pw_dir)] = '\0';
    strncat(filePath, "/sktFiles/", strlen("/sktFiles/"));
    std::cout << "filePath: " << filePath << std::endl;
    struct stat st = {0};
    mkdir(filePath, 0777);
    if (stat(filePath, &st) == -1)
        error("error al crear");
    strncat(filePath, fileName, strlen(fileName));
    std::cout << "filePath: " << filePath << std::endl;
}

void *thread_proc(void *arg) {
    int clientSocket = *((int *) (&arg));
    FileInfo fileInfo;
    if (recv(clientSocket, &fileInfo, sizeof(fileInfo), 0) < 0)
        error("cannot read file size");
    char filePath[256];
    buildFilePath(filePath, fileInfo.fileName);
    readFileBytes(clientSocket, filePath, fileInfo.fileSize);
    close(clientSocket);
    return nullptr;
}

int main() {
    std::cout << "server started" << std::endl;
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
