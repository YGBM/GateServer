#include "mysqlDao.hpp"


connection_pool::connection_pool()
{
    m_CurConn = 0;
    m_FreeConn = 0;
}

connection_pool *connection_pool::GetInstance()
{
    static connection_pool connPool;
    return &connPool;
}

//构造初始化
void connection_pool::init(std::string url, std::string User, std::string PassWord, std::string DBName, int Port, int MaxConn, int close_log)
{
    m_url = url;
    m_Port = Port;
    m_User = User;
    m_PassWord = PassWord;
    m_DatabaseName = DBName;
    m_close_log = close_log;

    for (int i = 0; i < MaxConn; i++)
    {
        MYSQL *con = NULL;
        con = mysql_init(con);

        if (con == NULL)
        {
//            LOG_ERROR("MySQL Error");
            exit(1);
        }
        if (!mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0))
        {
            std::cout << mysql_errno(con) << "   " << mysql_error(con) << std::endl;
            exit(1);
        }
        connList.push_back(con);
        ++m_FreeConn;
    }

//    reserve = sem(m_FreeConn);

    m_MaxConn = m_FreeConn;
}


//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *connection_pool::GetConnection()
{
    MYSQL *con = NULL;

    if (0 == connList.size())
        return NULL;

//    reserve.wait();

//    lock.lock();

    con = connList.front();
    connList.pop_front();

    --m_FreeConn;
    ++m_CurConn;

//    lock.unlock();
    return con;
}

//释放当前使用的连接
bool connection_pool::ReleaseConnection(MYSQL *con)
{
    if (NULL == con)
        return false;

//    lock.lock();

    connList.push_back(con);
    ++m_FreeConn;
    --m_CurConn;

//    lock.unlock();

//    reserve.post();
    return true;
}

//销毁数据库连接池
void connection_pool::DestroyPool()
{

//    lock.lock();
    if (connList.size() > 0)
    {
        std::list<MYSQL *>::iterator it;
        for (it = connList.begin(); it != connList.end(); ++it)
        {
            MYSQL *con = *it;
            mysql_close(con);
        }
        m_CurConn = 0;
        m_FreeConn = 0;
        connList.clear();
    }

//    lock.unlock();
}

//当前空闲的连接数
int connection_pool::GetFreeConn()
{
    return this->m_FreeConn;
}

connection_pool::~connection_pool()
{
    DestroyPool();
}

MysqlDao::MysqlDao()
{
//    auto & cfg = ConfigMgr::Inst();
    const auto& host = "192.168.3.15";//cfg["Mysql"]["Host"];
    const auto& port =3306 ;//cfg["Mysql"]["Port"];
    const auto& pwd ="aiops";// cfg["Mysql"]["Passwd"];
    const auto& schema = "aiops";//cfg["Mysql"]["Schema"];
    const auto& user ="aiops";// cfg["Mysql"]["User"];


    pool_.reset(connection_pool::GetInstance());
    pool_->init(host,user,pwd,schema,port,5,0);

//    pool_.reset(new MySqlPool(host+":"+port, user, pwd,schema, 5));
}

MysqlDao::~MysqlDao(){
    pool_->DestroyPool();
}

int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    return 1;
//    auto con = pool_->getConnection();
//    try {
//        if (con == nullptr) {
//            return false;
//        }
//        // 准备调用存储过程
//        unique_ptr < sql::PreparedStatement > stmt(con->_con->prepareStatement("CALL reg_user(?,?,?,@result)"));
//        // 设置输入参数
//        stmt->setString(1, name);
//        stmt->setString(2, email);
//        stmt->setString(3, pwd);
//
//        // 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
//
//        // 执行存储过程
//        stmt->execute();
//        // 如果存储过程设置了会话变量或有其他方式获取输出参数的值，你可以在这里执行SELECT查询来获取它们
//        // 例如，如果存储过程设置了一个会话变量@result来存储输出结果，可以这样获取：
//        unique_ptr<sql::Statement> stmtResult(con->_con->createStatement());
//        unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
//        if (res->next()) {
//            int result = res->getInt("result");
//            cout << "Result: " << result << endl;
//            pool_->returnConnection(std::move(con));
//            return result;
//        }
//        pool_->returnConnection(std::move(con));
//        return -1;
//    }
//    catch (sql::SQLException& e) {
//        pool_->returnConnection(std::move(con));
//        std::cerr << "SQLException: " << e.what();
//        std::cerr << " (MySQL error code: " << e.getErrorCode();
//        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
//        return -1;
//    }
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
    MYSQL * con = pool_->GetConnection();

    mysql_query(con,"select * from demo");
    MYSQL_RES *result = mysql_store_result(con);
    int num_fields = mysql_num_fields(result);
    //返回所有字段结构的数组
    MYSQL_FIELD *fields = mysql_fetch_fields(result);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    MYSQL_ROW row = mysql_fetch_row(result);
    std::string temp1(row[0]);
    std::cout << temp1 << std::endl;

    return false;
//    auto con = pool_->getConnection();
//    try {
//        if (con == nullptr) {
//            return false;
//        }
//
//        // 准备查询语句
//        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT email FROM user WHERE name = ?"));
//
//        // 绑定参数
//        pstmt->setString(1, name);
//
//        // 执行查询
//        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
//
//        // 遍历结果集
//        while (res->next()) {
//            std::cout << "Check Email: " << res->getString("email") << std::endl;
//            if (email != res->getString("email")) {
//                pool_->returnConnection(std::move(con));
//                return false;
//            }
//            pool_->returnConnection(std::move(con));
//            return true;
//        }
//        return true;
//    }
//    catch (sql::SQLException& e) {
//        pool_->returnConnection(std::move(con));
//        std::cerr << "SQLException: " << e.what();
//        std::cerr << " (MySQL error code: " << e.getErrorCode();
//        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
//        return false;
//    }
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd) {
    return false;
//    auto con = pool_->getConnection();
//    try {
//        if (con == nullptr) {
//            return false;
//        }
//
//        // 准备查询语句
//        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
//
//        // 绑定参数
//        pstmt->setString(2, name);
//        pstmt->setString(1, newpwd);
//
//        // 执行更新
//        int updateCount = pstmt->executeUpdate();
//
//        std::cout << "Updated rows: " << updateCount << std::endl;
//        pool_->returnConnection(std::move(con));
//        return true;
//    }
//    catch (sql::SQLException& e) {
//        pool_->returnConnection(std::move(con));
//        std::cerr << "SQLException: " << e.what();
//        std::cerr << " (MySQL error code: " << e.getErrorCode();
//        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
//        return false;
//    }
}

bool MysqlDao::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo) {
    return false;
//    auto con = pool_->getConnection();
//    if (con == nullptr) {
//        return false;
//    }
//
//    Defer defer([this, &con]() {
//        pool_->returnConnection(std::move(con));
//    });
//
//    try {
//
//
//        // 准备SQL语句
//        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE email = ?"));
//        pstmt->setString(1, email); // 将username替换为你要查询的用户名
//
//        // 执行查询
//        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
//        std::string origin_pwd = "";
//        // 遍历结果集
//        while (res->next()) {
//            origin_pwd = res->getString("pwd");
//            // 输出查询到的密码
//            std::cout << "Password: " << origin_pwd << std::endl;
//            break;
//        }
//
//        if (pwd != origin_pwd) {
//            return false;
//        }
//        userInfo.name = res->getString("name");
//        userInfo.email = email;
//        userInfo.uid = res->getInt("uid");
//        userInfo.pwd = origin_pwd;
//        return true;
//    }
//    catch (sql::SQLException& e) {
//        std::cerr << "SQLException: " << e.what();
//        std::cerr << " (MySQL error code: " << e.getErrorCode();
//        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
//        return false;
//    }
}


