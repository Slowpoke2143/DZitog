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
    void loadUsersFromFile();  // ���������� ������� �������� �������������
    void saveUsersToFile();  // ���������� ������� ���������� �������������

    int _port;
    int _serverSocket;
    std::map<std::string, std::string> users;  // �������� �������������
    std::map<int, std::string> clientUsernames;  // �������� �������������, ������������ � �������
};

#endif // TCPSERVER_H
