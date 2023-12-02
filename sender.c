#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 

#define PORT 8080
#define SIZE_of_SEGMENT 1024

void sendFile(const char* fileName, int sockett) {
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        perror("Error in opening file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char buffer[SIZE_of_SEGMENT];
    size_t numSegments = fileSize / SIZE_of_SEGMENT;
    size_t lastSegment = fileSize % SIZE_of_SEGMENT;
    send(sockett, &fileSize, sizeof(long), 0);

    for (size_t i = 0; i < numSegments; i++) {
        fread(buffer, sizeof(char), SIZE_of_SEGMENT, file);
        send(sockett, buffer, SIZE_of_SEGMENT, 0);
    }

    if (lastSegment > 0) {
        fread(buffer, sizeof(char), lastSegment, file);
        send(sockett, buffer, lastSegment, 0);
    }

    fclose(file);
    close(sockett);
}

int main() {
    struct sockaddr_in serverAddress;
    int sockett = socket(AF_INET, SOCK_STREAM, 0);

    if (sockett == -1) {
        perror("Failure in socket creation");
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(PORT);

    if (connect(sockett, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0) {
        perror("Connection with the server failed");
        exit(1);
    }

    sendFile("my_audio.wav", sockett);

    return 0;
}