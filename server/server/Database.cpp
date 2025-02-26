#include "DataBase.h"

bool DataBase::connect() {
    // Выделяем дескриптор для базы данных
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
        return false;

    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
        return false;

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
        return false;

    std::cout << "Attempting connection to SQL Server...\n";

    // Устанавливаем соединение с сервером
    switch (SQLDriverConnect(sqlConnHandle,
        nullptr,
        (SQLWCHAR*)L"DRIVER={MySQL ODBC 9.2 ANSI Driver};SERVER=localhost;PORT=3306;DATABASE=chatdb;UID=root;PWD=root;",
        SQL_NTS,
        retconstring,
        1024,
        nullptr,
        SQL_DRIVER_COMPLETE)) {

    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
        std::cout << "Successfully connected to SQL Server\n";
        return true;

    case SQL_INVALID_HANDLE:
    case SQL_ERROR:
        std::cerr << "Could not connect to SQL Server\n";
        return false;

    default:
        return false;
    }
}

void DataBase::disconnect() {
    if (sqlStmtHandle) SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    if (sqlConnHandle) SQLDisconnect(sqlConnHandle);
    if (sqlConnHandle) SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    if (sqlEnvHandle) SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
}

bool DataBase::registerUser(const std::string& username, const std::string& password) {
    if (!connect()) return false;

    std::wstring query = L"INSERT INTO users (username, password) VALUES ('" +
        std::wstring(username.begin(), username.end()) + L"', '" +
        std::wstring(password.begin(), password.end()) + L"');";

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))
        return false;

    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        std::cerr << "Error executing query: " << query.c_str() << std::endl;
        return false;
    }

    std::cout << "User registered: " << username << std::endl;
    disconnect();
    return true;
}

bool DataBase::authenticateUser(const std::string& username, const std::string& password) {
    if (!connect()) return false;

    std::wstring query = L"SELECT password FROM users WHERE username = '" +
        std::wstring(username.begin(), username.end()) + L"';";

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))
        return false;

    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        std::cerr << "Error executing query: " << query.c_str() << std::endl;
        return false;
    }

    SQLCHAR sqlPassword[SQL_RESULT_LEN];
    SQLLEN passwordLength;
    if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        SQLGetData(sqlStmtHandle, 1, SQL_C_CHAR, sqlPassword, SQL_RESULT_LEN, &passwordLength);
        std::string dbPassword(reinterpret_cast<char*>(sqlPassword), passwordLength);
        disconnect();
        return dbPassword == password;
    }

    disconnect();
    return false;
}