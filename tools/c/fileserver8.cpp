/*
 *  程序名：fileserver8.cpp，文件传输的服务端。
    把tcpputfiles.cpp发送文件的功能抄过来，放在SendFilesMain中。
 *  作者：吴从周
*/
#include "_public.h"
 
CLogFile logfile;       // 服务程序的运行日志。
CTcpServer TcpServer;   // 创建服务端对象。

// 程序退出时调用的函数
void FathEXIT(int sig);   // 父进程退出函数。
void ChldEXIT(int sig);   // 子进程退出函数。

char strRecvBuffer[1024];  // 发送报文的buffer。
char strSendBuffer[1024];  // 接收报文的buffer。 

// 登录客户端的登录报文。
bool ClientLogin();

// 接收文件主函数
void RecvFilesMain();

// 接收对端发送过来的文件内容。
bool RecvFile(const int sockfd,const char *filename,const char *mtime,const int filesize);

// 发送文件的主函数。
bool _tcpputfiles();
bool bcontinue=true;   // 如果调用_tcpputfiles发送了文件，bcontinue为true，初始化为true。

// 把文件的内容发送给对端。
bool SendFile(const int sockfd,const char *filename,const char *mtime,const int filesize);

// 接收到对端的文件传输确认报文。
bool _ackresult(char *buffer);

// 发送文件主函数
void SendFilesMain();

// 向对端发送心跳报文
bool ActiveTest();

struct st_arg
{
  int  clienttype;          // 客户端类型，1-发送文件；2-接收文件。
  char ip[31];              // 服务器端的IP地址。
  int  port;                // 服务器端的端口。
  int  ptype;               // 文件发送成功后文件的处理方式：1-删除文件；2-移动到备份目录。
  char clientpath[301];     // 本地文件存放的根目录。
  bool andchild;            // 是否发送clientpath目录下各级子目录的文件，true-是；false-否。
  char matchname[301];      // 待发送文件名的匹配方式，如"*.TXT,*.XML"，注意用大写。
  char srvpath[301];        // 服务端文件存放的根目录。
  char srvpathbak[301];     // 服务端文件的备份目录。
  int  timetvl;             // 扫描本地目录文件的时间间隔，单位：秒。
} starg;

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer);

int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./fileserver8 port logfile\nExample:./fileserver8 5005 /tmp/fileserver8.log\n\n"); return -1;
  }

  // 关闭全部的信号和输入输出
  // CloseIOAndSignal();

  // 处理程序退出的信号
  signal(SIGINT,FathEXIT); signal(SIGTERM,FathEXIT);

  if (logfile.Open(argv[2],"a+")==false)
  {
    printf("打开日志文件失败（%s）。\n",argv[1]); return -1;
  }
 
  if (TcpServer.InitServer(atoi(argv[1]))==false) // 初始化TcpServer的通信端口。
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n",argv[1]); FathEXIT(-1);
  }
 
  while (true)
  {
    if (TcpServer.Accept()==false)   // 等待客户端连接。
    {
      logfile.Write("TcpServer.Accept() failed.\n"); continue;
    }

    /*
    // 注释这段代码，暂时采用单进程，方便调试。
    if (fork()>0) { TcpServer.CloseClient(); continue; } // 父进程返回到循环首部。

    // 子进程重新设置退出信号。
    signal(SIGINT,ChldEXIT); signal(SIGTERM,ChldEXIT);

    TcpServer.CloseListen();
    */
 
    // 以下是子进程，负责与客户端通信。
    logfile.Write("客户端(%s)已连接。\n",TcpServer.GetIP());
 
    // 登录客户端的登录报文。
    if (ClientLogin() == false) ChldEXIT(0);

    // 接收文件主函数
    if (starg.clienttype==1) RecvFilesMain();

    // 发送文件主函数
    if (starg.clienttype==2) SendFilesMain();

    ChldEXIT(-1);  // 通信完成后，子进程退出。
  }
}

// 父进程退出时调用的函数
void FathEXIT(int sig)
{
  if (sig > 0)
  {
    signal(sig,SIG_IGN); signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);
    logfile.Write("catching the signal(%d).\n",sig);
  }

  kill(0,15);  // 通知其它的子进程退出。

  logfile.Write("父进程退出。\n");

  // 编写善后代码（释放资源、提交或回滚事务）
  TcpServer.CloseClient();

  exit(0);
}

// 子进程退出时调用的函数
void ChldEXIT(int sig)
{
  if (sig > 0)
  {
    signal(sig,SIG_IGN); signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);
  }

  logfile.Write("子进程退出。\n");

  // 编写善后代码（释放资源、提交或回滚事务）
  TcpServer.CloseClient();

  exit(0);
}

// 登录客户端的登录报文。
bool ClientLogin()
{
  memset(strRecvBuffer,0,sizeof(strRecvBuffer));
  memset(strSendBuffer,0,sizeof(strSendBuffer));

  if (TcpServer.Read(strRecvBuffer,20) == false)
  {
    logfile.Write("1 TcpServer.Read() failed.\n"); return false;
  }
  logfile.Write("1 strRecvBuffer=%s\n",strRecvBuffer);  // xxxxxx

  // 解析客户端登录报文。
  _xmltoarg(strRecvBuffer);

  if ( (starg.clienttype==1) || (starg.clienttype==2) )
    strcpy(strSendBuffer,"ok");
  else
    strcpy(strSendBuffer,"failed");

  // logfile.Write("1 strSendBuffer=%s\n",strSendBuffer);  // xxxxxx
  if (TcpServer.Write(strSendBuffer) == false)
  {
    logfile.Write("1 TcpServer.Write() failed.\n"); return false;
  }

  logfile.Write("%s login %s.\n",TcpServer.GetIP(),strSendBuffer);

  return true;
}

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"clienttype",&starg.clienttype);

  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);

  GetXMLBuffer(strxmlbuffer,"clientpath",starg.clientpath);

  GetXMLBuffer(strxmlbuffer,"andchild",&starg.andchild);

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname);

  GetXMLBuffer(strxmlbuffer,"srvpath",starg.srvpath);

  GetXMLBuffer(strxmlbuffer,"srvpathbak",starg.srvpathbak);

  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);

  if (starg.timetvl>50) starg.timetvl=50;

  return true;
}

// 接收对端发送过来的文件内容。
bool RecvFile(const int sockfd,const char *filename,const char *mtime,const int filesize)
{
  char strfilenametmp[301]; memset(strfilenametmp,0,sizeof(strfilenametmp));
  sprintf(strfilenametmp,"%s.tmp",filename);

  FILE *fp=NULL;

  if ( (fp=FOPEN(strfilenametmp,"wb")) ==NULL) return false;

  int  total_bytes=0;
  int  onread=0;
  char buffer[1000];

  while (true)
  {
    memset(buffer,0,sizeof(buffer));

    if ((filesize-total_bytes) > 1000) onread=1000;
    else onread=filesize-total_bytes;

    if (Readn(sockfd,buffer,onread) == false) { fclose(fp); fp=NULL; return false; }

    fwrite(buffer,1,onread,fp);

    total_bytes = total_bytes + onread;

    if ((int)total_bytes == filesize) break;
  }

  fclose(fp);

  // 重置文件的时间
  UTime(strfilenametmp,mtime);

  if (RENAME(strfilenametmp,filename)==false) return false;

  return true;
}

// 接收文件主函数
void RecvFilesMain()
{
  while (true)
  {
    memset(strRecvBuffer,0,sizeof(strRecvBuffer));
    memset(strSendBuffer,0,sizeof(strSendBuffer));

    if (TcpServer.Read(strRecvBuffer,80) == false)
    {
      logfile.Write("2 TcpServer.Read() failed.\n"); ChldEXIT(-1);
    }
    // logfile.Write("2 strRecvBuffer=%s\n",strRecvBuffer);  // xxxxxx

    // 处理心跳报文
    if (strstr(strRecvBuffer,"activetest")!=0)
    {
      strcpy(strSendBuffer,"ok");
      // logfile.Write("2 strSendBuffer=%s\n",strSendBuffer);  // xxxxxx
      if (TcpServer.Write(strSendBuffer) == false)
      {
        logfile.Write("2 TcpServer.Write() failed.\n"); ChldEXIT(-1);
      }

      continue;
    }

    // 如果报文是接收文件请求。
    if (strstr(strRecvBuffer,"<filename>")!=0)
    {
      // 这里将插入接收文件内容的代码。
      // 解析待接收的文件的时间和大小
      char clientfilename[301]; memset(clientfilename,0,sizeof(clientfilename));
      char mtime[21]; memset(mtime,0,sizeof(mtime));
      int  filesize=0;
      GetXMLBuffer(strRecvBuffer,"filename",clientfilename,300);
      GetXMLBuffer(strRecvBuffer,"mtime",mtime,20);
      GetXMLBuffer(strRecvBuffer,"size",&filesize);

      // 客户端和服务端文件的目录是不一样的，以下代码生成服务端的文件名。
      // 把文件名中的clientpath替换成srvpath，要小心第三个参数
      char serverfilename[301]; memset(serverfilename,0,sizeof(serverfilename));
      strcpy(serverfilename,clientfilename);
      UpdateStr(serverfilename,starg.clientpath,starg.srvpath,false);

      logfile.Write("recv %s(%d) ...",serverfilename,filesize);
      // 接收对端发送过来的文件内容。
      if (RecvFile(TcpServer.m_connfd,serverfilename,mtime,filesize)==true)
      {
        logfile.WriteEx("ok.\n");
        SNPRINTF(strSendBuffer,1000,1000,"<filename>%s</filename><result>ok</result>",clientfilename);
      }
      else
      {
        logfile.WriteEx("failed.\n");
        SNPRINTF(strSendBuffer,1000,1000,"<filename>%s</filename><result>failed</result>",clientfilename);
      }

      // 把接收结果返回给对端。
      // logfile.Write("3 strSendBuffer=%s\n",strSendBuffer);  // xxxxxx
      if (TcpServer.Write(strSendBuffer) == false)
      {
        logfile.Write("3 TcpServer.Write() failed.\n"); ChldEXIT(-1);
      }

      continue;
    }
  }
}

// 向对端发送心跳报文
bool ActiveTest()
{
  memset(strRecvBuffer,0,sizeof(strRecvBuffer));
  memset(strSendBuffer,0,sizeof(strSendBuffer));

  strcpy(strSendBuffer,"<activetest>ok</activetest>");

  // logfile.Write("strSendBuffer=%s\n",strSendBuffer);  // xxxxxx
  if (TcpServer.Write(strSendBuffer) == false)
  {
    logfile.Write("2 TcpServer.Write() failed.\n"); TcpServer.CloseClient(); return false;
  }

  if (TcpServer.Read(strRecvBuffer,20) == false)
  {
    logfile.Write("2 TcpServer.Read() failed.\n"); TcpServer.CloseClient(); return false;
  }
  // logfile.Write("strRecvBuffer=%s\n",strRecvBuffer);  // xxxxxx

  if (strcmp(strRecvBuffer,"ok") != 0) { TcpServer.CloseClient(); return false; }

  return true;
}

// 发送文件主函数
void SendFilesMain()
{
  while (true)
  {
    // 发送文件的主函数。
    if (_tcpputfiles()==false) { logfile.Write("_tcpputfiles() failed.\n"); return; }

    if (bcontinue==false)
    {
      // sleep(starg.timetvl); // 用以下循环代替sleep(starg.timetvl)、防止timeout<timetvl的情况。
      for (int ii=0;ii<starg.timetvl;ii++)
      {
        sleep(1); // PActive.UptATime();
      }

      if (ActiveTest()==false) break;
    }
  }
}

// 发送文件的主函数。
bool _tcpputfiles()
{
  CDir Dir;

  if (Dir.OpenDir(starg.srvpath,starg.matchname,10000,starg.andchild,false)==false)
  {
    logfile.Write("Dir.OpenDir(%s) 失败。\n",starg.srvpath); return false;
  }

  CTimer Timer;

  // 未收到对端确认报文的文件数量。
  int delayed=0;

  bcontinue=false;

  while (true)
  {
    // 遍历目录中的每个文件。
    if (Dir.ReadDir()==false) break;

    bcontinue=true;

    // logfile.Write("filename=%s,mtime=%s,size=%d\n",Dir.m_FullFileName,Dir.m_ModifyTime,Dir.m_FileSize);

    // 把文件名、修改时间、文件大小组成报文，发送给对端。
    SNPRINTF(strSendBuffer,1000,1000,"<filename>%s</filename><mtime>%s</mtime><size>%d</size>",Dir.m_FullFileName,Dir.m_ModifyTime,Dir.m_FileSize);

    // logfile.Write("3 strSendBuffer=%s\n",strSendBuffer);  // xxxxxx
    if (TcpServer.Write(strSendBuffer) == false)
    {
      logfile.Write("3 TcpServer.Write() failed.\n"); TcpServer.CloseClient(); return false;
    }

    // 把文件的内容发送给对端。
     logfile.Write("send %s(%d) ...",Dir.m_FullFileName,Dir.m_FileSize);
    if (SendFile(TcpServer.m_connfd,Dir.m_FullFileName,Dir.m_ModifyTime,Dir.m_FileSize)==true)
    {
      logfile.WriteEx("ok.\n"); delayed++;
    }
    else
    {
      logfile.WriteEx("failed.\n");
      TcpServer.CloseClient();
      return false;
    }

    // PActive.UptATime();

    // 以不阻塞的方式接收对端返回的结果。
    int buflen=0;
    memset(strRecvBuffer,0,sizeof(strRecvBuffer));
    if (TcpRead(TcpServer.m_connfd,strRecvBuffer,&buflen,-1) == true)
    {
      delayed--;
      _ackresult(strRecvBuffer); // 接收到对端的文件传输确认报文。
      // logfile.Write("1strRecvBuffer=%s\n",strRecvBuffer);  // xxxxxx
    }
  }

  // 等待剩余未收到对端确认的报文。
  for (int ii=0;ii<delayed;ii++)
  {
    // 接收对端返回的结果。
    memset(strRecvBuffer,0,sizeof(strRecvBuffer));
    if (TcpServer.Read(strRecvBuffer,20) == true)
    {
      _ackresult(strRecvBuffer); // 接收到对端的文件传输确认报文。
      // logfile.Write("2strRecvBuffer=%s\n",strRecvBuffer);  // xxxxxx
    }
  }

  // logfile.Write("累计时长%.2f秒。\n",Timer.Elapsed());

  return true;
}

// 接收到对端的文件传输确认报文。
bool _ackresult(char *buffer)
{
  char ackfilename[301];
  char ackresult[11];

  memset(ackfilename,0,sizeof(ackfilename));
  memset(ackresult,0,sizeof(ackresult));

  GetXMLBuffer(buffer,"filename",ackfilename,300);
  GetXMLBuffer(buffer,"result",ackresult,10);

  logfile.Write("%s %s.\n",ackfilename,ackresult);

  // 删除文件。
  if (starg.ptype==1)
  {
    if (REMOVE(ackfilename)==false)
    {
      logfile.Write("REMOVE(%s) failed.\n",ackfilename); return false;
    }
  }

  // 备份文件。
  if (starg.ptype==2)
  {
    char bakfilename[301];
    STRCPY(bakfilename,sizeof(bakfilename),ackfilename);
    UpdateStr(bakfilename,starg.srvpath,starg.srvpathbak,false);
    if (RENAME(ackfilename,bakfilename)==false)
    {
      logfile.Write("RENAME(%s,%s) failed.\n",ackfilename,bakfilename); return false;
    }
  }

  return true;
}


// 把文件的内容发送给对端。
bool SendFile(const int sockfd,const char *filename,const char *mtime,const int filesize)
{
  int  bytes=0;
  int  total_bytes=0;
  int  onread=0;
  char buffer[1000];

  FILE *fp=NULL;

  if ( (fp=FOPEN(filename,"rb")) == NULL ) return false;

  while (true)
  {
    memset(buffer,0,sizeof(buffer));

    if ((filesize-total_bytes) > 1000) onread=1000;
    else onread=filesize-total_bytes;

    bytes=fread(buffer,1,onread,fp);

    if (bytes > 0)
    {
      if (Writen(sockfd,buffer,bytes) == false) { fclose(fp); fp=NULL; return false; }
    }

    total_bytes = total_bytes + bytes;

    if ((int)total_bytes == filesize) break;
  }

  fclose(fp);

  return true;
}

