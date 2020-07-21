#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


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
pthread_cond_t queue_not_fu11 ;
/*当任务队列满时，添加任务的线程阻塞，等待此条件变量*/
pthread_cond_t queue_not_empty;
/*任务队列里不为空时，通知等待任务的线程*/
pthread_t *threads;
/*存放线程池中每个线程的tid。数组*/
pthread_t adjust_tid;
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


void threadpool_free(threadpool_t *pool);

void* threadpool_thread(void * threadpool)
{
    threadpool_t *pool = (threadpool_t *) threadpool;
    while (true)
    {
        printf("thread 0x%x is waiting \n", (unsigned int)pthread_self());
        while((pool->queue_size == 0) && (!pool->shutdown))
        {
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));

            //清除指定数目的空闲线程，如果要结束的线程个数大于0,结束线程
            if(pool->wait_exit_thr_num > 0)
            {
                pool->wait_exit_thr_num--;

                //如果线程池里的个数大于最小值时可以结束当前线程
                if(pool->live_thr_num > pool->min_thr_num)
                {
                    printf("thread 0x %x is exitting \n", (unsigned int)pthread_self());
                    pool->live_thr_num--;
                    pthread_mutex_lock(&(pool->lock));

                    pthread_exit(NULL);
                }
            }
        }
    }
    

}

void* adjust_thead(void * threadpool)
{
    int i;
    threadpool_t *pool = (threadpool_t *) threadpool;
    while (!pool->shutdown)
    {
        sleep(10);  //定时线程管理
        pthread_mutex_lock(&(pool->lock));
        int live_thr_num = pool->live_thr_num;   //存活线程数
        pthread_mutex_unlock(&(pool->lock));

        pthread_mutex_lock(&(pool->thread_counter));   //忙线程
        int busy_thr_num = pool->busy_thr_num;
        pthread_mutex_unlock(&(pool->thread_counter));

        //创建线程算法：任务数大于最小线程 并且 存活线程数小于最大线程个数时 如 30 

    }
    
}

threadpool_t* threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size)
{
    int i;
    threadpool_t *pool = NULL;
    do
    {
        
        if((pool = (threadpool_t*)malloc(sizeof(threadpool_t))) == NULL)
        {
            break;
        }

        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->busy_thr_num = 0;
        pool->live_thr_num = 0;
        pool->wait_exit_thr_num = 0;
        pool->queue_size = queue_max_size;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->shutdown = 0;

        //工作线程数组开辟空间，并清0
        pool->threads = (pthread_t *)malloc(sizeof(threadpoo1_task_t) * queue_max_size);
        if(pool->threads == NULL)
        {
            break;
        }
            
        memset(pool->threads, 0, sizeof(pthread_t)*max_thr_num);

        //任务队列开辟空间
        pool->task_queue = (threadpoo1_task_t *)malloc(sizeof(threadpoo1_task_t) * queue_max_size);
        if(pool->task_queue == NULL)
        {
            break;
        }

        //条件变量
        if(pthread_mutex_init(&(pool->lock), NULL) != 0
           || pthread_mutex_init(&(pool->thread_counter), NULL) != 0
           || pthread_cond_init(&(pool->queue_not_empty), NULL) != 0
           || pthraed_cond_init(&(pool->queue_not_fu11), NULL) != 0)
        {
            break;
        }

        //启动min_thr_num个work thread
        for(i = 0; i < min_thr_num; i++)
        {
            pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
            printf("start thread 0x %x \n", (unsigned int)pool->threads[i]);
        }

        //创建管理者线程
        pthread_create(&(pool->adjust_tid), NULL, adjust_thead, (void *)pool);

        return pool;

    } while (0);
    
    threadpool_free(pool);

    return NULL;
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