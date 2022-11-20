# CPP数据中心
### 主要学习参考了吴哥的[cpp气象数据中心](https://coding.imooc.com/learn/list/546.html)。框架参考了[ccfree](https://github.com/zhuzhenxxx/ccfree)
#### 本人学习的代码存放在pthread1, idc1, tools1中。所有代码都可以使用makefile轻松编译测试。
* public:存放框架
* pthread1:演示线程使用 [makefile](pthread1/makefile)
* idc1:生成数据 [makefile](idc1/c/makefile)
  * [start.sh](idc1/c/start.sh) 启动脚本
  * [killall.sh](idc1/c/start.sh) 删除服务脚本
  * [crtsurfdata5.cpp](idc1/c/crtsurfdata5.cpp) 用于生成全国气象站点观察的分钟数据
  * [obtcodetodb.cpp](idc1/c/obtcodetodb.cpp) 本程序用于把全国站点参数数据保存到数据库T_ZHOBTCODE表中
  * [obtmindtodb.cpp](idc1/c/obtmindtodb.cpp) 本程序用于把全国站点分钟观测数据入库到T_ZHOBTMIND表中，支持xml和csv两种文件格式
* tools1:工具程序 [makefile](tools1/c/makefile)
  * [checproc.cpp](tools1/c/checkproc.cpp) 守护进程
  * [deletefiles.cpp](tools1/c/deletefiles.cpp) 用于删除历史的数据文件或日志文件
  * [gzipfiles.cpp](tools1/c/gzipfiles.cpp) 用于压缩历史的数据文件或日志文件
  * [procctl.cpp](tools1/c/procctl.cpp) 服务程序的调度程序，周期性启动服务程序或shell脚本
  * ftp模块
    * [ftpgetfiles.cpp](tools1/c/ftpgetfiles.cpp) 调用ftp获取服务器文件
    * [ftpputfiles.cpp](tools1/c/ftpputfiles.cpp) ftp上传文件
  * tcp模块
    * [tcpputfiles.cpp](tools1/c/tcpputfiles.cpp) 采用tcp协议，实现文件上传的客户端
    * [tcpgetfiles.cpp](tools1/c/tcpgetfiles.cpp) 采用tcp协议，实现文件下载的客户端
    * [fileserver.cpp](tools1/c/fileserver.cpp) 文件传输的服务
  * mysql模块
    * [diminingmysql.cpp](tools1/c/dminingmysql.cpp) 本程序是数据中心的公共功能模块，用于从mysql数据库源表抽取数据，生成xml文件
    * [xmltodb.cpp](tools1/c/xmltodb.cpp) 本程序是数据中心的公共功能模块，用于把xml文件入库到MySQL的表中
    * [migratetable.cpp](tools1/c/migratetable.cpp) 本程序是数据中心的公共功能模块，用于迁移表中的数据
    * [deletetable.cpp](tools1/c/deletetable.cpp) 本程序是数据中心的公共功能模块，用于定时清理表中的数据
    * [syncupdate.cpp](tools1/c/syncupdate.cpp) 本程序是数据中心的公共功能模块，采用刷新的方法同步MySQL数据库之间的表
    * [syncincrement.cpp](tools1/c/syncincrement.cpp) 本程序是数据中心的公共功能模块，采用增量的方法同步MySQL数据库之间的表
    * [syncincrementex.cpp](tools1/c/syncincrementex.cpp) 本程序是数据中心的公共功能模块，采用增量的方法同步MySQL数据库之间的表
      * 本程序不使用Federated引擎
  * oracle模块
    * [dminingoracle.cpp](tools1/c/dminingoracle.cpp) 本程序是数据中心的公共功能模块，用于从Oracle数据库源表抽取数据，生成xml文件
    * [xmltodb_oracle.cpp](tools1/c/xmltodb_oracle.cpp) 本程序是数据中心的公共功能模块，用于把xml文件入库到Oracle的表中
    * [migratetable_oracle.cpp](tools1/c/migratetable_oracle.cpp) 本程序是数据中心的公共功能模块，用于迁移表中的数据
    * [deletetable_oracle.cpp](tools1/c/deletetable_oracle.cpp) 本程序是数据中心的公共功能模块，用于定时清理表中的数据
    * [syncupdate_oracle.cpp](tools1/c/syncupdate_oracle.cpp) 本程序是数据中心的公共功能模块，采用刷新的方法同步Oracle数据库之间的表
    * [syncincrement_oracle.cpp](tools1/c/syncincrement_oracle.cpp) 本程序是数据中心的公共功能模块，采用增量的方法同步Oracle数据库之间的表
    * [syncincrementex_oracle.cpp](tools1/c/syncincrementex_oracle.cpp) 本程序是数据中心的公共功能模块，采用增量的方法同步Oracle数据库之间的表。
      *  注意，本程序不使用dblink。
  * 数据服务总线
    * [webserver.cpp](tools1/c/webserver.cpp) 此程序是数据服务总线的服务端程序
      * 实践了线程池和数据库连接池
  * 代理模块
    * [inetd.cpp](tools1/c/inetd.cpp) 网络代理服务程序
      * 正向代理
    * [rinetd.cpp](tools1/c/rinetd.cpp) 网络代理服务程序-外网端
      * 反向代理外网端
    * [rinetdin.cpp](tools1/c/rinetdin.cpp) 网络代理服务程序-内网端
      * 反向代理内网端
  