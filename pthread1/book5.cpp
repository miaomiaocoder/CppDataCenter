// 本程序演示线程线程退出（终止）的状态。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thmain(void *arg); // 线程的主函数。



int main(int argc, char *argv[])
{
  pthread_t thid = 0;

  // 创建线程。
  if (pthread_create(&thid, NULL, thmain, NULL) != 0)
  {
    printf("pthread_create failed.\n");
    exit(-1);
  }

  // 等待子线程退出。
  printf("join...\n");
  void *pv=0;
  pthread_join(thid, &pv);
  printf("ret=%ld\n", (long)pv);
  printf("join ok.\n");
}

void *thmain(void *arg) // 线程主函数。
{
  printf("线程开始运行。\n");
  pthread_exit ((void * )213);
}
