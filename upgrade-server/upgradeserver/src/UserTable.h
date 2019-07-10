/**
 * @file UserTable.h
 * @brief
 * @author  <itarge@itarge.com>
 * @version 1.0.0
 * @date 2018-01-10
 */
/* Copyright(C) 2009-2017, Itarge Inc.
 * All right reserved
 *
 */

#ifndef __USER_TABLE_H_
#define __USER_TABLE_H_
#include <vector>
using namespace std;

class CUserTable
{
    static const int sc_userinfolength = 32;    //用户名称和口令的最大长度
    typedef struct UserInfo
    {
        char userName[sc_userinfolength];
        char PWD[sc_userinfolength];
    } UserInfo, *PUserInfo;

    CUserTable();

public:
    ~CUserTable(void);

    static CUserTable *instance();

public:
    int UserIsValid(char *szUserName, char *szPWD);
    bool InitUserTable(char* szUserFileName);

private:
    vector<UserInfo> vUserTable;
};
#endif /* __USER_TABLE_H_ */
