#include <iostream>
#include <hiredis.h>
#include "./hv/HttpServer.h"
#include "document.h"
#include "fwd.h"
#include "stringbuffer.h"
#include "writer.h"
#include "redisMgr.hpp"
#include <mysql/mysql.h>
#include "mysqlMgr.hpp"

using namespace hv;

void testServer(){

    HttpService router;
    router.GET("/ping", [](HttpRequest* req, HttpResponse* resp) {
        return resp->String("pong");
    });


    router.GET("/data", [](HttpRequest* req, HttpResponse* resp) {
        static char data[] = "0123456789";
        return resp->Data(data, 10);
    });

    router.GET("/paths", [&router](HttpRequest* req, HttpResponse* resp) {
        return resp->Json(router.Paths());
    });

    router.GET("/get", [](HttpRequest* req, HttpResponse* resp) {
        resp->json["origin"] = req->client_addr.ip;
        resp->json["url"] = req->url;
        resp->json["args"] = req->query_params;
        resp->json["headers"] = req->headers;
//        bool ree = MysqlMgr::GetInstance()->CheckEmail("fuzusheng","fuzusheng@gmail.com");
        return 200;
    });

    router.POST("/echo", [](const HttpContextPtr& ctx) {
        return ctx->send(ctx->body(), ctx->type());
    });



    HttpServer server(&router);
    server.setPort(8080);
    server.setThreadNum(4);
    server.run();
}

using namespace rapidjson;

void testjson(){
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);
    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    std::cout << buffer.GetString() << std::endl;
}

void test_redis(){
    redisContext* c = redisConnect("192.168.3.15",6379);
    if (c->err){
        redisFree(c);
        return;
    }
    auto r = redisCommand(c,"set name fuzusheng");

}

void test_redis_cli(){
    RedisMgr::GetInstance();
    RedisMgr::GetInstance()->Set("key1","ddd");
}

void test_mysql() {
    MYSQL *con = NULL;
    con = mysql_init(con);
    if (!mysql_real_connect(con, "192.168.3.15", "aiops", "aiops", "aiops", 3306, NULL, 0)) {
        cout << mysql_errno(con) << "   " << mysql_error(con) << endl;
        exit(1);
    }
    mysql_query(con,"select * from demo");
    MYSQL_RES *result = mysql_store_result(con);
    int num_fields = mysql_num_fields(result);
    //返回所有字段结构的数组
    MYSQL_FIELD *fields = mysql_fetch_fields(result);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while (MYSQL_ROW row = mysql_fetch_row(result))
    {
        string temp1(row[0]);
        std::cout << temp1 << std::endl;
    }
}
 int main() {
//    MysqlMgr::GetInstance();
//    testServer();
//    test_mysql();
     bool ree = MysqlMgr::GetInstance()->CheckEmail("fuzusheng","fuzusheng@gmail.com");
    return 0;
}
