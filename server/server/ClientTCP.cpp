#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "ClientTCP.h"
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

TCPClient::TCPClient(const std::string& serverIp, int port)
    : _serverIp(serverIp), _port(port), _socket(INVALID_SOCKET) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
    }
}

bool TCPClient::connectToServer() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(_port);

    if (inet_pton(AF_INET, _serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        closesocket(_socket);
        return false;
    }

    if (connect(_socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Connection failed!" << std::endl;
        closesocket(_socket);
        return false;
    }

    std::cout << "Connected to the server!" << std::endl;
    return true;
}

void TCPClient::sendMessage(const std::string& message) {
    logger.log("You: " + message); // Логируем отправленное сообщение
    if (send(_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Message send failed!" << std::endl;
    }
    else {
        std::cout << "Message sent: " << message << std::endl;
    }
}

void TCPClient::receiveMessage() {
    char buffer[1024];
    int bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::string receivedMessage(buffer);
        logger.log("Server: " + receivedMessage); // Логируем полученное сообщение
        std::cout << "Received: " << receivedMessage << std::endl;
    }
    else if (bytesReceived == 0) {
        std::cout << "Connection closed by server." << std::endl;
    }
    else {
        std::cerr << "Failed to receive message!" << std::endl;
    }
}

void TCPClient::run() {
    std::string message;
    while (true) {
        receiveMessage();
        std::cout << "Enter message to send: " << std::endl;
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }
        sendMessage(message);
    }
    closesocket(_socket);
    WSACleanup();
}