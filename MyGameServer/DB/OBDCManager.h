#pragma once
#include <sqlext.h>  
class OBDCManager
{
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;
    SQLRETURN retcode;


    SQLLEN cb_x = 0, cb_y = 0, cb_id = 0, cb_name = 0;
public:
    SQLINTEGER user_lv = -1;
    SQLINTEGER user_hp;
    SQLINTEGER user_max_hp;
    SQLINTEGER user_exp;
    SQLINTEGER user_x;
    SQLINTEGER user_y;
    SQLWCHAR user_name[10];
    OBDCManager() {};
    ~OBDCManager() {};
    void AllocateHandles();
    void ConnectDataSource(const SQLWCHAR* obdc);
    void ExecuteStatementDirect(const SQLWCHAR* sql);
    void ExecuteStatement(const SQLWCHAR* exec);
    void RetrieveResult();
    void DisconnectDataSource();
    void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
};

