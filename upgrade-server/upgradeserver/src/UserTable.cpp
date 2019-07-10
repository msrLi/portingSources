#include "ace/OS_main.h"
#include "ace/FILE_Addr.h"
#include "ace/FILE_Connector.h"
#include "ace/FILE_IO.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_stdio.h"
#include "UserTable.h"

CUserTable::CUserTable()
{
}

CUserTable::~CUserTable(void)
{


}


CUserTable * CUserTable::instance()
{
    static  CUserTable UserTable;
    return &UserTable;
}

int CUserTable::UserIsValid(char *szUserName, char *szPWD)
{
    if (!szUserName || !szPWD)
    {
        return -1;
    }

    vector<UserInfo>::iterator it = vUserTable.begin();
    for (; it != vUserTable.end(); it++)
    {

        if (!strcmp(it->userName, szUserName) && !strcmp(it->PWD, szPWD))
        {
            return 0;
        }
    }
    //     return 0;
    return -1;
}

bool CUserTable::InitUserTable(char* szUserFileName)
{
    UserInfo testinfo;
    ACE_OS::strcpy(testinfo.userName, "iTarge");
    ACE_OS::strcpy(testinfo.PWD, "iTarge");
    vUserTable.push_back(testinfo);

    return true;
}
