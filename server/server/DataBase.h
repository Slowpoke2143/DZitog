#pragma once
#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>

class DataBase
{
private:
    static constexpr auto SQL_RESULT_LEN = 240;
    static constexpr auto SQL_RETURN_CODE_LEN = 1024;

    SQLHANDLE sqlConnHandle{ nullptr }; // дескриптор для соединения с базой данных
    SQLHANDLE sqlEnvHandle{ nullptr }; // дескриптор окружения базы данных
    SQLHANDLE sqlStmtHandle{ nullptr };  // дескриптор для выполнения запросов к базе данных
    SQLWCHAR retconstring[SQL_RETURN_CODE_LEN]{}; // строка для кода возврата из функций API ODBC

public:
    DataBase() = default;
    ~DataBase() = default;

    bool connect(); // Устанавливаем соединение с базой данных
    void disconnect(); // Закрываем соединение с базой данных

    bool registerUser(const std::string& username, const std::string& password); // Регистрация пользователя
    bool authenticateUser(const std::string& username, const std::string& password); // Аутентификация пользователя
};