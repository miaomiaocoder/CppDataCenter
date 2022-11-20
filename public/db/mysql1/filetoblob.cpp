/*
 *  ��������filetoblob.cpp���˳�����ʾ������ܲ���MySQL���ݿ⣨��ͼƬ�ļ�����BLOB�ֶΣ���
 *  ���ߣ�����ܡ�
*/
#include "_mysql.h"   // ������ܲ���MySQL��ͷ�ļ���

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// �������ڳ�Ů��Ϣ�Ľṹ������е��ֶζ�Ӧ��
struct st_girls
{
  long id;          // ��Ů��ţ���long�������Ͷ�ӦOracle��С����number(10)��
  char name[31];    // ��Ů��������char[31]��ӦOracle��varchar2(30)��
  double weight;    // ��Ů���أ���double�������Ͷ�ӦOracle��С����number(8,2)��
  char btime[20];   // ����ʱ�䣬��char��ӦOracle��date����ʽ��'yyyy-mm-dd hh24:mi:ssi'��
  char pic[100000]; // ��ŮͼƬ��
  unsigned long picsize; // ͼƬ��С��
} stgirls;

int main(int argc,char *argv[])
{
  connection conn; // ���ݿ�������

  // ��¼���ݿ⣬����ֵ��0-�ɹ�������-ʧ�ܡ�
  // ʧ�ܴ�����conn.m_cda.rc�У�ʧ��������conn.m_cda.message�С�
  if (conn.connecttodb("127.0.0.1,root,mysqlpwd,mysql,3306","gbk") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }
  
  sqlstatement stmt(&conn); // ����SQL���Ķ���

  // ׼��������SQL��䡣
  stmt.prepare("update girls set pic=:1 where id=:2");
  // prepare��������Ҫ�жϷ���ֵ��
  // ΪSQL������������ĵ�ַ��bindin��������Ҫ�жϷ���ֵ��
  stmt.bindinlob(1,stgirls.pic,&stgirls.picsize);
  stmt.bindin(2,&stgirls.id);

  stgirls.id=1;                                 // ��Ů��š�

  // ��ͼƬ�����ݼ��ص�pic�С�
  stgirls.picsize=filetobuf("pic_in.jpeg",stgirls.pic);

  // ִ��SQL��䣬һ��Ҫ�жϷ���ֵ��0-�ɹ�������-ʧ�ܡ�
  // ʧ�ܴ�����stmt.m_cda.rc�У�ʧ��������stmt.m_cda.message�С�
  if (stmt.execute() != 0)
  {
      printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
  }

  printf("�ɹ�������%ld����¼��\n",stmt.m_cda.rpc); // stmt.m_cda.rpc�Ǳ���ִ��SQLӰ��ļ�¼����

  printf("update table girls ok.\n");

  conn.commit(); // �ύ���ݿ�����
}

