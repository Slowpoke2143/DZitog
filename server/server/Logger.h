#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <mutex>
#include <shared_mutex>

class Logger {
public:
    Logger(const std::string& filename = "log.txt") {
        logFile.open(filename, std::ios::app); // Открываем файл для добавления логов
        if (!logFile.is_open()) {
            throw std::runtime_error("Failed to open log file!");
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close(); // Закрываем файл при разрушении объекта
        }
    }

    // Метод для записи строки в лог
    void log(const std::string& message) {
        std::unique_lock<std::shared_mutex> lock(mutex); // Блокировка для записи
        if (logFile.is_open()) {
            logFile << message << std::endl; // Записываем сообщение в файл
        }
    }

    // Метод для чтения строки из лога
    std::string readLine() {
        std::shared_lock<std::shared_mutex> lock(mutex); // Блокировка для чтения
        std::string line;
        if (logFile.is_open()) {
            std::ifstream inFile("log.txt"); // Открываем файл для чтения
            if (inFile.is_open()) {
                std::getline(inFile, line); // Читаем одну строку
            }
        }
        return line;
    }

private:
    std::fstream logFile; // Файл для логов
    mutable std::shared_mutex mutex; // Мьютекс для потокобезопасности
};

#endif // LOGGER_H