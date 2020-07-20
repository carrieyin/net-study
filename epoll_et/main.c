#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
void *(*function) (void *);
/*函数指针，回调函数*/
void *arg;
/* .上面函数的参数*/
}threadpoo1_task_t;
I
/*各子线程任务结构体*/;

typedef struct {
pthread_mutex_t lock;
/*用于锁住本结构体*/
pthread_mutex_t thread_counter;
/*记录忙状态线程个数de琐-- busy__thr__num */
pthread_cond_t queue__not__fu11 ;
/*当任务队列满时，添加任务的线程阻塞，等待此条件变量*/
pthread_cond_t queue_not__empty;
/*任务队列里不为空时，通知等待任务的线程*/
pthread_t *threads;
/*存放线程池中每个线程的tid。数组*/
pthread_t adjust__tid;
/*存管理线程tid. */
threadpoo1_task_t *task_queue;
/*任务队列(数组首地址) */
int min_thr_num;
/*线程池最小线程数*/
int max_thr_num;
/*线程池最大线程数*/
int live_thr_num;
/*当前存活线程个数*/
int busy_thr_num;
/*忙状态线程个数*/
int wait_exit_thr_num;
/*要销毁的线程个数*/
int queue_front ;
/* task_.queue队头下标*/
int queue_rear ;
/* task__queue队尾下标*/
int queue_size;
/* task__queue队中实际任务数. */
int queue_max_size;
/* task__queue队列可容纳任务数上限*/
int shutdown;
/*标志位，线程池使用状态，true或fa1se */
} threadpool_t;

threadpool_t* threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size)
{
    int i;
    threadpool_t *pool = NULL;
    do
    {
        
        if((pool = (threadpool_t*)malloc(sizeof(threadpool_t))) == NULL)
        {

        }
    } while (0);
    
}

int main()
{
    threadpool_t *thp = threadpool_create(3, 100, 100);                  /*创建线程池，最小3个线程，最大100, 队列最大100*/
    printf("pool inited \n");

    int num[20], i;
    for(i = 0; i < 20; i++)
    {
        num[i] = i;
        printf("add task %d \n",i);
        threadpool_add(thp, process, (void *)&num);
    }

    sleep(10);
    treadpool_destory(thp);
;}