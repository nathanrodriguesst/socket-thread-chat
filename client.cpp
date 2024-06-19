#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

void receiveMessages(int clientSocket) {
    char buffer[1024];
    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == -1) {
            std::cerr << "Failed to receive data." << std::endl;
            break;
        } else if (bytesRead == 0) {
            std::cout << "Disconnected." << std::endl;
            break;
        } else {
            buffer[bytesRead] = '\0'; // Null-terminate the buffer
            std::cout << "Server: " << buffer << std::endl;

            if (strcmp(buffer, "cmd:kick") == 0) {
                std::cout << "You've been kicked from the server.\n";
                break;
            }
        }
    }
    close(clientSocket);
}

void sendMessages(int clientSocket) {
    char message[1024];
    while (true) {
        std::cin.getline(message, 1024);

        std::cout << "Me: " << message << std::endl;

        send(clientSocket, message, strlen(message), 0);

        if (strcmp(message, "cmd:quit") == 0) {
            std::cout << "Quitting connection..." << std::endl;
            break;
        }
    }
    close(clientSocket);
}

void connection() {
    std::cout << "Connecting to server..." << std::endl;
    sleep(2);

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5568);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return;
    }

    std::cout << "Connected as client. Start talking!" << std::endl;

    std::thread sender(sendMessages, clientSocket);
    std::thread receiver(receiveMessages, clientSocket);

    sender.join();
    receiver.join();
}

int main() {
    connection();
    return 0;
}