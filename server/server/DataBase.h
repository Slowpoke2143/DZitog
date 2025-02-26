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

    SQLHANDLE sqlConnHandle{ nullptr }; // ���������� ��� ���������� � ����� ������
    SQLHANDLE sqlEnvHandle{ nullptr }; // ���������� ��������� ���� ������
    SQLHANDLE sqlStmtHandle{ nullptr };  // ���������� ��� ���������� �������� � ���� ������
    SQLWCHAR retconstring[SQL_RETURN_CODE_LEN]{}; // ������ ��� ���� �������� �� ������� API ODBC

public:
    DataBase() = default;
    ~DataBase() = default;

    bool connect(); // ������������� ���������� � ����� ������
    void disconnect(); // ��������� ���������� � ����� ������

    bool registerUser(const std::string& username, const std::string& password); // ����������� ������������
    bool authenticateUser(const std::string& username, const std::string& password); // �������������� ������������
};