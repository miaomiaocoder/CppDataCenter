/*
 *  程序名：createtable.cpp，此程序演示开发框架操作MySQL数据库（创建表）。
 *  作者：任振华。
 */

#include "_mysql.h" // 开发框架操作MySQL的头文件。

int main(int argc, char *argv[])
{
    connection conn; // 数据库连接类。

    // 登录数据库，返回值：0-成功，其它-失败。
    // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
    if (conn.connecttodb("127.0.0.1,root,123456,ren,3306", "utf8") != 0)
    {
        printf("connect database failed.\n%s\n", conn.m_cda.message);
        return -1;
    }

    sqlstatement stmt(&conn); // 操作SQL语句的对象。

    // 准备创建表的SQL语句。
    // 超女表girls，超女编号id，超女姓名name，体重weight，报名时间btime，超女说明memo，超女图片pic。
    stmt.prepare("create table girls(id      bigint(10),\
                   name    varchar(30),\
                   weight  decimal(8,2),\
                   btime   datetime,\
                   memo    longtext,\
                   pic     longblob,\
                   primary key (id))");

    //   1、int prepare(const char *fmt,...)，SQL语句可以多行书写。
    //   2、SQL语句最后的分号可有可无，建议不要写（兼容性考虑）。
    //   3、SQL语句中不能有说明文字。
    //   4、可以不用判断stmt.prepare()的返回值，stmt.execute()时再判断。

    // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
    // 失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中。
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
        return -1;
    }

    printf("create table girls ok.\n");

    return 0;
}
