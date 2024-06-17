#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <thread>

void receiveMessages(int clientSocket) {
    char buffer[1024] = { 0 };
    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == -1) {
            std::cerr << "Failed to receive data\n";
            break;
        } else if (bytesRead == 0) {
            // Client closed the connection
            std::cout << "Client disconnected\n";
            break;
        } else {
            buffer[bytesRead] = '\0'; // Null-terminate the buffer
            std::cout << "Client: " << buffer << std::endl;

            // If the client sends "cmd:STOP", you can handle it as needed
            if (strcmp(buffer, "cmd:STOP") == 0) {
                std::cout << "Client requested to stop the server.\n";
                break;
            }
        }
    }
    close(clientSocket);
}

void sendMessages(int clientSocket) {
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "cmd:STOP") {
            std::cout << "Stopping server...\n";
            send(clientSocket, message.c_str(), message.size(), 0);
            break;
        }

        std::cout << "Me: " << message << std::endl;

        send(clientSocket, message.c_str(), message.size(), 0);
    }
    close(clientSocket);
}

void connection() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    std::cout << "Starting server..." << std::endl;
    sleep(2);

    if (serverSocket == -1) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(27908);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket\n";
        close(serverSocket);
        return;
    }

    std::cout << "Listening for client connections..." << std::endl;
    sleep(2);

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Failed to listen on socket\n";
        close(serverSocket);
        return;
    }

    std::cout << "Connected as server. Start talking!" << std::endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept connection\n";
            break;
        }

        // Create a thread to receive messages from the client
        std::thread receiver(receiveMessages, clientSocket);

        // Create a thread to send messages to the client
        std::thread sender(sendMessages, clientSocket);

        // Join the threads before accepting a new client
        receiver.join();
        sender.join();
    }

    close(serverSocket);
}

int main() {
    connection();
    return 0;
}