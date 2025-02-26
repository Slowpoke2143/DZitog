#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>

class TCPClient {
public:
    TCPClient(const std::string& serverIp, int port);
    bool connectToServer();
    void sendMessage(const std::string& message);
    void receiveMessage();
    void run();

private:
    std::string _serverIp;
    int _port;
    int _socket;
};

#endif // TCPCLIENT_H

#pragma once
