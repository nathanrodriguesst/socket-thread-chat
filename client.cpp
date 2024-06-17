#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

void sendMessage(int clientSocket) {
    char message[1024];
    while (true) {
        std::cin.getline(message, 1024);

        std::cout << "Me: " << message << std::endl;

        send(clientSocket, message, strlen(message), 0);

        if (strcmp(message, "cmd:STOP") == 0) {
            std::cout << "Stopping client..." << std::endl;
            break;
        }
    }
    close(clientSocket);
}

void receiveMessage(int clientSocket) {
    char buffer[1024];
    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == -1) {
            std::cerr << "Failed to receive data" << std::endl;
            break;
        } else if (bytesRead == 0) {
            std::cout << "Server disconnected" << std::endl;
            break;
        } else {
            buffer[bytesRead] = '\0'; // Null-terminate the buffer
            std::cout << "Server: " << buffer << std::endl;
        }
    }
    close(clientSocket);
}

void connection() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(27908);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return;
    }

    std::cout << "Connected as client. Start talking!" << std::endl;

    std::thread sender(sendMessage, clientSocket);
    std::thread receiver(receiveMessage, clientSocket);

    sender.join();
    receiver.join();
}

int main() {
    std::cout << "Connecting to server..." << std::endl;
    sleep(2);
    connection();
    return 0;
}
