#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <mysql/mysql.h>
#include <list>


//
//class SqlConnection {
//public:
//    SqlConnection(MYSQL* con, int64_t lasttime):_con(con), _last_oper_time(lasttime){}
//    std::unique_ptr<MYSQL> _con;
//    int64_t _last_oper_time;
//};
//
//class MySqlPool {
//public:
//    MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize)
//            : url_(url), user_(user), pass_(pass), schema_(schema), poolSize_(poolSize), b_stop_(false){
//        try {
//            for (int i = 0; i < poolSize_; ++i) {
//                MYSQL *con = NULL;
//                con = mysql_init(con);
//                mysql_real_connect(con, url.c_str(), user.c_str(), pass.c_str(), schema.c_str(), 3306, NULL, 0)
////                sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
////                auto*  con = driver->connect(url_, user_, pass_);
////                con->setSchema(schema_);
//                // 获取当前时间戳
//                auto currentTime = std::chrono::system_clock::now().time_since_epoch();
//                // 将时间戳转换为秒
//                long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
//                pool_.push(std::make_unique<SqlConnection>(con, timestamp));
//            }
//
//            _check_thread = 	std::thread([this]() {
//                while (!b_stop_) {
//                    checkConnection();
//                    std::this_thread::sleep_for(std::chrono::seconds(60));
//                }
//            });
//
//            _check_thread.detach();
//        }
//        catch (std::exception& e) {
//            // 处理异常
//            std::cout << "mysql pool init failed, error is " << e.what()<< std::endl;
//        }
//    }
//
//    void checkConnection() {
//        std::lock_guard<std::mutex> guard(mutex_);
//        int poolsize = pool_.size();
//        // 获取当前时间戳
//        auto currentTime = std::chrono::system_clock::now().time_since_epoch();
//        // 将时间戳转换为秒
//        long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
//        for (int i = 0; i < poolsize; i++) {
//            auto con = std::move(pool_.front());
//            pool_.pop();
//            Defer defer([this, &con]() {
//                pool_.push(std::move(con));
//            });
//
//            if (timestamp - con->_last_oper_time < 5) {
//                continue;
//            }
//
//            try {
//                std::unique_ptr<sql::Statement> stmt(con->_con->createStatement());
//                stmt->executeQuery("SELECT 1");
//                con->_last_oper_time = timestamp;
//                //std::cout << "execute timer alive query , cur is " << timestamp << std::endl;
//            }
//            catch (sql::SQLException& e) {
//                std::cout << "Error keeping connection alive: " << e.what() << std::endl;
//                // 重新创建连接并替换旧的连接
//                sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
//                auto* newcon = driver->connect(url_, user_, pass_);
//                newcon->setSchema(schema_);
//                con->_con.reset(newcon);
//                con->_last_oper_time = timestamp;
//            }
//        }
//    }
//
//    std::unique_ptr<SqlConnection> getConnection() {
//        std::unique_lock<std::mutex> lock(mutex_);
//        cond_.wait(lock, [this] {
//            if (b_stop_) {
//                return true;
//            }
//            return !pool_.empty(); });
//        if (b_stop_) {
//            return nullptr;
//        }
//        std::unique_ptr<SqlConnection> con(std::move(pool_.front()));
//        pool_.pop();
//        return con;
//    }
//
//    void returnConnection(std::unique_ptr<SqlConnection> con) {
//        std::unique_lock<std::mutex> lock(mutex_);
//        if (b_stop_) {
//            return;
//        }
//        pool_.push(std::move(con));
//        cond_.notify_one();
//    }
//
//    void Close() {
//        b_stop_ = true;
//        cond_.notify_all();
//    }
//
//    ~MySqlPool() {
//        std::unique_lock<std::mutex> lock(mutex_);
//        while (!pool_.empty()) {
//            pool_.pop();
//        }
//    }
//
//private:
//    std::string url_;
//    std::string user_;
//    std::string pass_;
//    std::string schema_;
//    int poolSize_;
//    std::queue<std::unique_ptr<SqlConnection>> pool_;
//    std::mutex mutex_;
//    std::condition_variable cond_;
//    std::atomic<bool> b_stop_;
//    std::thread _check_thread;
//};
class connection_pool
{
public:
    MYSQL *GetConnection();				 //获取数据库连接
    bool ReleaseConnection(MYSQL *conn); //释放连接
    int GetFreeConn();					 //获取连接
    void DestroyPool();					 //销毁所有连接

    //单例模式
    static connection_pool *GetInstance();

    void init(std::string url, std::string User, std::string PassWord, std::string DataBaseName, int Port, int MaxConn, int close_log);
    ~connection_pool();
private:
    connection_pool();


    int m_MaxConn;  //最大连接数
    int m_CurConn;  //当前已使用的连接数
    int m_FreeConn; //当前空闲的连接数
    std::list<MYSQL *> connList; //连接池

public:
    std::string m_url;			 //主机地址
    std::string m_Port;		 //数据库端口号
    std::string m_User;		 //登陆数据库用户名
    std::string m_PassWord;	 //登陆数据库密码
    std::string m_DatabaseName; //使用数据库名
    int m_close_log;	//日志开关
};

struct UserInfo {
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};

class MysqlDao
{
public:
    MysqlDao();
    ~MysqlDao();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string & email);
    bool UpdatePwd(const std::string& name, const std::string& newpwd);
    bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
private:
    std::unique_ptr<connection_pool> pool_;
};


