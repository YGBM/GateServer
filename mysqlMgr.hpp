#pragma once
#include "mysqlDao.hpp"
#include "Singleton.hpp"

class MysqlMgr: public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email,  const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string & email);
    bool UpdatePwd(const std::string& name, const std::string& email);
    bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
private:
    MysqlMgr();
    MysqlDao  _dao;
};

