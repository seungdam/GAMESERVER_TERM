#pragma once
#include <windows.h>  
#include <iostream>  
#include <sqlext.h>  
class OBDC_MGR
{
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;
    SQLRETURN retcode;


    SQLLEN cb_x = 0, cb_y = 0, cb_id = 0, cb_name = 0;
public:
    SQLINTEGER user_id = -1;
    SQLINTEGER user_x;
    SQLINTEGER user_y;
    SQLWCHAR user_name[10];
    OBDC_MGR() {};
    ~OBDC_MGR() {};
    void AllocateHandles();
    void ConnectDataSource(const SQLWCHAR* obdc);
    void ExecuteStatementDirect(const SQLWCHAR* sql);
    void ExecuteStatement(const SQLWCHAR* exec);
    void RetrieveResult();
    void DisconnectDataSource();
    void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
};

