#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <map>

class TCPServer {
public:
    TCPServer(int port);
    bool start();
    void acceptClients();
    void handleClient(int clientSocket);
    void sendToAllClients(const std::string& message);
    void stopServer();

private:
    void loadUsersFromFile();  // Декларация функции загрузки пользователей
    void saveUsersToFile();  // Декларация функции сохранения пользователей

    int _port;
    int _serverSocket;
    std::map<std::string, std::string> users;  // Хранение пользователей
    std::map<int, std::string> clientUsernames;  // Хранение пользователей, подключенных к серверу
};

#endif // TCPSERVER_H
