#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// Source - https://stackoverflow.com/a/32286531
// Posted by bames53, modified by community. See post 'Timeline' for change history
// Retrieved 2026-04-15, License - CC BY-SA 4.0

#include <filesystem>

uintmax_t getFileSize(const char* filename) {
    return std::filesystem::file_size(filename);
}

int main() {
    char fileName[] = "mdiscretas.pdf";
    std::cout << "client started" << std::endl;
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
    char filePath[256];
    strcpy(filePath, "/home/imaxii/qt-workspace/sktServer/");
    strcat(filePath, fileName);
    std::cout << filePath << std::endl;
    uintmax_t fileSize = getFileSize(filePath);
    std::cout << fileSize << std::endl;
    send(clientSocket, &fileSize, sizeof(uintmax_t), 0);
    send(clientSocket, fileName, sizeof(fileName), 0);
    std::cout << sizeof(fileName) << std::endl;
    if (FILE *fp = fopen(filePath, "rb")) {
        size_t readBytes;
        char buffer[8192];
        while ((readBytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            if (send(clientSocket, buffer, readBytes, 0) != readBytes) {
                std::cout << "error" << std::endl;
                //handleErrors();
                break;
            }
        }
        fclose(fp);
        // closing socket
        close(clientSocket);
    }
    return 0;
}
