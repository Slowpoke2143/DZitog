#include "ServerTCP.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <thread>
#include <fstream>
#include "DataBase.h"

#pragma comment(lib, "ws2_32.lib")

TCPServer::TCPServer(int port)
    : _port(port), _serverSocket(INVALID_SOCKET) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
    }
}

bool TCPServer::start() {
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Binding failed!" << std::endl;
        closesocket(_serverSocket);
        return false;
    }

    if (listen(_serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed!" << std::endl;
        closesocket(_serverSocket);
        return false;
    }

    std::cout << "Server started, waiting for clients..." << std::endl;
    return true;
}

void TCPServer::acceptClients() {
    while (true) {
        int clientSocket = accept(_serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Client connection failed!" << std::endl;
        }
        else {
            std::cout << "Client connected!" << std::endl;
            std::thread(&TCPServer::handleClient, this, clientSocket).detach();
        }
    }
}

void TCPServer::handleClient(int clientSocket) {
    DataBase db;
    char buffer[1024];
    bool loggedIn = false;
    std::string username, password;

    while (true) {
        if (!loggedIn) {
            send(clientSocket, "1. Register\n2. Login\n", 22, 0);
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                std::cerr << "Client disconnected or error occurred." << std::endl;
                break;
            }

            std::string input(buffer);
            input.erase(input.find_last_not_of(" \n\r\t") + 1);

            int choice = 0;
            try {
                choice = std::stoi(input);
            }
            catch (const std::exception& e) {
                send(clientSocket, "Invalid input. Please enter a number.\n", 36, 0);
                continue;
            }

            switch (choice) {
            case 1: {
                send(clientSocket, "Enter username: ", 17, 0);
                memset(buffer, 0, sizeof(buffer));
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) break;
                username = std::string(buffer, bytesReceived);
                username.erase(username.find_last_not_of(" \n\r\t") + 1);

                send(clientSocket, "Enter password: ", 17, 0);
                memset(buffer, 0, sizeof(buffer));
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) break;
                password = std::string(buffer, bytesReceived);
                password.erase(password.find_last_not_of(" \n\r\t") + 1);

                if (db.registerUser(username, password)) {
                    send(clientSocket, "Registration successful! Please log in.\n", 40, 0);
                }
                else {
                    send(clientSocket, "Registration failed. Try again.\n", 32, 0);
                }
                break;
            }
            case 2: {
                send(clientSocket, "Enter username: ", 17, 0);
                memset(buffer, 0, sizeof(buffer));
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) break;
                username = std::string(buffer, bytesReceived);
                username.erase(username.find_last_not_of(" \n\r\t") + 1);

                send(clientSocket, "Enter password: ", 17, 0);
                memset(buffer, 0, sizeof(buffer));
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) break;
                password = std::string(buffer, bytesReceived);
                password.erase(password.find_last_not_of(" \n\r\t") + 1);

                if (db.authenticateUser(username, password)) {
                    clientUsernames[clientSocket] = username;
                    send(clientSocket, "Login successful.\n", 18, 0);
                    loggedIn = true;
                }
                else {
                    send(clientSocket, "Incorrect credentials. Please try again.\n", 41, 0);
                }
                break;
            }
            default: {
                send(clientSocket, "Invalid choice. Please try again.\n", 32, 0);
                break;
            }
            }
        }
        else {
            send(clientSocket, "1. Send message to chat\n2. Log out\n", 35, 0);
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                std::cerr << "Client disconnected or error occurred." << std::endl;
                break;
            }

            std::string input(buffer);
            input.erase(input.find_last_not_of(" \n\r\t") + 1);

            int action = 0;
            try {
                action = std::stoi(input);
            }
            catch (const std::exception& e) {
                send(clientSocket, "Invalid input. Please enter a number.\n", 36, 0);
                continue;
            }

            switch (action) {
            case 1: {
                send(clientSocket, "Enter message for chat: ", 24, 0);
                memset(buffer, 0, sizeof(buffer));
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) break;
                std::string message = std::string(buffer, bytesReceived);
                message.erase(message.find_last_not_of(" \n\r\t") + 1);

                sendToAllClients(username + ": " + message);
                break;
            }
            case 2: {
                send(clientSocket, "Logging out.\n", 13, 0);
                closesocket(clientSocket);
                clientUsernames.erase(clientSocket);
                std::cout << "Client disconnected: " << username << std::endl;
                return;
            }
            default: {
                send(clientSocket, "Invalid action. Please try again.\n", 32, 0);
                break;
            }
            }
        }
    }

    closesocket(clientSocket);
    clientUsernames.erase(clientSocket);
    std::cout << "Client disconnected: " << username << std::endl;
}

void TCPServer::sendToAllClients(const std::string& message) {
    logger.log(message); // Логируем сообщение
    for (auto& client : clientUsernames) {
        send(client.first, message.c_str(), message.length(), 0);
    }
}

void TCPServer::stopServer() {
    closesocket(_serverSocket);
    WSACleanup();
    std::cout << "Server stopped." << std::endl;
}