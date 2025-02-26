#include <iostream>
#include "ServerTCP.h"
#include "ClientTCP.h"
#include "DataBase.h"


int main() {

    int choice;
    std::cout << "Select mode:\n1. Start Server\n2. Start Client\n";
    std::cin >> choice;
    std::cin.ignore();  // Очищаем буфер после ввода

    if (choice == 1) {
        TCPServer server(8080);
        if (server.start()) {
            server.acceptClients();
        }
    }
    else if (choice == 2) {
        TCPClient client("127.0.0.1", 8080);  // Указываем IP и порт сервера
        if (client.connectToServer()) {
            client.run();
        }
    }
    else {
        std::cout << "Invalid choice!" << std::endl;
    }

    return 0;
}
