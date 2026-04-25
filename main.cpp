#include <fstream>
#include <iostream>
#include <netinet/in.h>
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

void readFileBytes(int clientSocket, const char fileName[256], uintmax_t &fileSize) {
    if (FILE *fp = fopen(fileName, "wb")) {
        size_t readBytes;
        char buffer[8192];
        while ((readBytes = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0 && fileSize > 0) {
            if (fwrite(buffer, 1, readBytes, fp) != readBytes) {
                std::cout << "error leyendo";
                break;
            }
            fileSize -= readBytes;
            std::cout << "Received " << readBytes << " bytes" << std::endl;
            std::cout << "file size subtracted " << fileSize << std::endl;
        }
        if (fileSize > 0)
            error("error al recibir el archivo,archivo incompleto");
        std::cout << "File transfer complete." << std::endl;
        fclose(fp);
    }
}

char *buildFilePath(const char fileName[]) {
    struct passwd *pw = getpwuid(getuid());
    char *filePath = new char[256];
    strncpy(filePath, pw->pw_dir, strlen(pw->pw_dir));
    filePath[strlen(pw->pw_dir)] = '\0';
    strcat(filePath, "/sktFiles/");
    std::cout << filePath << std::endl;
    struct stat st = {0};
    mkdir(filePath, 0777);
    if (stat(filePath, &st) == -1)
        error("error al crear");
    strcat(filePath, fileName);
    return filePath;
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
    listen(serverSocket, 5);
    socklen_t clilen = sizeof(cli_addr);
    int clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
    if (clientSocket < 0)
        error("ERROR on accept");

    uintmax_t fileSize;
    if (recv(clientSocket, &fileSize, sizeof(long), 0) < 0)
        error("cannot read file size");
    std::cout << fileSize << std::endl;

    char fileName[256];
    if (recv(clientSocket, &fileName, sizeof(fileName), 0) < 0)
        error("cannot read file name");
    std::cout << fileName << std::endl;
    char *filePath = buildFilePath(fileName);
    readFileBytes(clientSocket, filePath, fileSize);
    close(clientSocket);
    close(serverSocket);
    return 0;
}
