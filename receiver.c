#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 

#define PORT 8080
#define SIZE_of_SEGMENT 1024

void receiveFile(const char* fileName, int sockett) {
    FILE *file = fopen(fileName, "wb");
    if (file == NULL) {
        perror("Error in creating file");
        exit(1);
    }

    char buffer[SIZE_of_SEGMENT];
    long fileSize;
    recv(sockett, &fileSize, sizeof(long), 0);

    size_t numSegments = fileSize / SIZE_of_SEGMENT;
    size_t lastSegment = fileSize % SIZE_of_SEGMENT;

    for (size_t i = 0; i < numSegments; i++) {
        recv(sockett, buffer, SIZE_of_SEGMENT, 0);
        fwrite(buffer, sizeof(char), SIZE_of_SEGMENT, file);
    }

    if (lastSegment > 0) {
        recv(sockett, buffer, lastSegment, 0);
        fwrite(buffer, sizeof(char), lastSegment, file);
    }

    fclose(file);
    close(sockett);
}

int main() {
    struct sockaddr_in serverAddress, clientt;
    int sockett, connectt, length;
    sockett = socket(AF_INET, SOCK_STREAM, 0);

    if (sockett == -1) {
        perror("Failure in socket creation");
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);

    if ((bind(sockett, (struct sockaddr *)&serverAddress, sizeof(serverAddress))) != 0) {
        perror("Failure in socket binding");
        exit(1);
    }

    if ((listen(sockett, 5)) != 0) {
        perror("Server not listening");
        exit(1);
    }

    length = sizeof(clientt);
    connectt = accept(sockett, (struct sockaddr *)&clientt, &length);

    if (connectt < 0) {
        perror("Server not accepting");
        exit(1);
    }

    printf("Connection established.");
    receiveFile("received_audio_file", connectt);

    return 0;
}