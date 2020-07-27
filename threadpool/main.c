#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define MIN_WAIT_TSK_NUM 10
#define DEFAULT_THREAD_VARY 10
 
typedef struct {
void *(*function) (void *);
/*函数指针，回调函数*/
void *arg;
/* .上面函数的参数*/
}threadpool_task_t;

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
threadpool_task_t *task_queue;
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


int threadpool_free(threadpool_t *pool)
{
    if(pool == NULL)
    {
        return -1;
    }

    if(pool->task_queue)
    {
        free(pool->task_queue);
    }

    if(pool->threads)
    {
        free(pool->threads);
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_lock(&(pool->thread_counter));
        pthread_mutex_destroy(&(pool->thread_counter));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_fu11));
    }
    free(pool);
    pool = NULL;
    return 0;

}

void* threadpool_thread(void * threadpool)
{
    threadpool_t *pool = (threadpool_t *) threadpool;
    threadpool_task_t task;
    while (true)
    {
        printf("thread 0x%x is waiting \n", (unsigned int)pthread_self());
        printf("thread unlocked queue %d, %d\n", pool->queue_size, pool->shutdown);
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
        //如果指定了true,要关闭线程池里的每个线程，自行退出处理，------销毁线程池
        if(pool->shutdown)
        {
            pthread_mutex_lock(&(pool->lock));
            printf("thread 0x%x is exiting \n", (unsigned int)pthread_self());
            pthread_detach(pthread_self());
            pthread_exit(NULL); //线程自行结束
        }
        //从任务队列里获取任务，是一个出队操作
        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;
        printf("get a queue 0x%x, 0x%x\n", (int)task.function, (int)task.arg);


        //出队
        pool->queue_front = (pool->queue_front + 1)% pool->queue_max_size;
        pool->queue_size--;

        //通知可以有新的任务加进来
        pthread_cond_broadcast(&(pool->queue_not_fu11));

        //任务取出后，立即释放线程锁
        pthread_mutex_unlock(&(pool->lock));

        //执行任务
        printf("thread 0x%x start working \n", (unsigned int)pthread_self());
        //忙线程数量+1
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num++;
        printf("busy thr count[%d] \n", pool->busy_thr_num);
        pthread_mutex_unlock(&(pool->thread_counter));

        //执行业务处理任务
        (*(task.function))(task.arg);

        //任务结束处理
        printf("thread 0x%x word end \n", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num--;
        pthread_mutex_unlock(&(pool->thread_counter));

    }

    pthread_exit(NULL);
    
}
bool is_thread_alive(pthread_t* th)
{
    
}

void* adjust_thread(void * threadpool)
{
    int i;
    threadpool_t *pool = (threadpool_t *) threadpool;
    while (!pool->shutdown)
    {
        sleep(10);  //定时线程管理

        pthread_mutex_lock(&(pool->lock));
        int queue_size = pool->queue_size;
        int live_thr_num = pool->live_thr_num;   //存活线程数
        pthread_mutex_unlock(&(pool->lock));

        pthread_mutex_lock(&(pool->thread_counter));   //忙线程
        int busy_thr_num = pool->busy_thr_num;
        pthread_mutex_unlock(&(pool->thread_counter));

        //创建线程算法：任务数大于最小线程 并且 存活线程数小于最大线程个数时 如 30>=10 && 40 <=100
        if(queue_size >= MIN_WAIT_TSK_NUM && live_thr_num < pool->max_thr_num)
        {
            pthread_mutex_lock(&(pool->lock));
            int add = 0;
            //一次增加 DEFAULT_THREAD 个线程
            for(i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY && 
                pool->live_thr_num < pool->max_thr_num; i++)
            {
                //if(pool->threads[i] || !is_thread_alive(pool->threads[i]))
                if(pool->threads[i])
                {
                    pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
                    add++;
                    pool->live_thr_num++;
                }
            }
            pthread_mutex_unlock(&(pool->lock));

        } 

        //忙线程×2 小于存活的线程数，且存活的线程数大于 最小线程数时
        if(busy_thr_num * 2 < live_thr_num && live_thr_num > pool->min_thr_num)
        {
            //一次销毁的线程数
            pthread_mutex_lock(&(pool->lock));
            pool->wait_exit_thr_num = DEFAULT_THREAD_VARY;
            pthread_mutex_unlock(&(pool->lock));

            for(i = 0; i < DEFAULT_THREAD_VARY; i ++)
            {
                pthread_cond_signal(&(pool->queue_not_empty));
            }
        }

    }

    return NULL;    
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
        pool->queue_size = 0;
        pool->queue_max_size = queue_max_size;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->shutdown = 0;

        //工作线程数组开辟空间，并清0
        pool->threads = (pthread_t *)malloc(sizeof(threadpool_task_t) * queue_max_size);
        if(pool->threads == NULL)
        {
            break;
        }
            
        memset(pool->threads, 0, sizeof(pthread_t)*max_thr_num);

        //任务队列开辟空间
        pool->task_queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t) * queue_max_size);
        if(pool->task_queue == NULL)
        {
            break;
        }

        //条件变量
        if(pthread_mutex_init(&(pool->lock), NULL) != 0
           || pthread_mutex_init(&(pool->thread_counter), NULL) != 0
           || pthread_cond_init(&(pool->queue_not_empty), NULL) != 0
           || pthread_cond_init(&(pool->queue_not_fu11), NULL) != 0)
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
        pthread_create(&(pool->adjust_tid), NULL, adjust_thread, (void *)pool);

        return pool;

    } while (0);
    
    threadpool_free(pool);
    return NULL;
}


int threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg)
{
    pthread_mutex_lock(&(pool->lock));
    
    //队列满，阻塞
    while ((pool->queue_size == pool->queue_max_size) && (!pool->shutdown))
    {
        pthread_cond_wait(&(pool->queue_not_fu11), &(pool->lock));
    }
     printf("add ... \n");
    if(pool->shutdown)
    {
        pthread_cond_broadcast(&(pool->queue_not_empty));
        pthread_mutex_unlock(&(pool->lock));
        return 0;

    }
    printf("task 清空 \n");
    //清空工作相称 调用的回调函数的参数的 arg
    if(pool->task_queue[pool->queue_rear].arg != NULL)
    {
        pool->task_queue[pool->queue_rear].arg = NULL;
    }

    printf("task[%d] start add\n", pool->queue_rear);
    //添加任务到任务队列里
    pool->task_queue[pool->queue_rear].function = function;
    printf("add function \n");
    pool->task_queue[pool->queue_rear].arg = arg;
    printf("add arg and set queue[%d]\n", pool->queue_max_size);
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;
    printf("update rear \n");
    pool->queue_size++;          //向任务队列中添加一个任务
    printf("set a queue 0x%x, 0x%x\n", (int)function, (int)arg);
    //添加任务后，队列不为空，唤醒线程池中的等待处理任务的线程
    pthread_cond_signal(&pool->queue_not_empty);
    pthread_mutex_unlock(&(pool->lock));
    printf("add over \n");
    return 0;
}

//具体业务处理在此实现
void *process(void *arg)
{
    printf("thread 0x%x working on task %d \n", (unsigned int)pthread_self(), (int)arg);
    sleep(1);                  //模拟业务处理
    printf("task %d is end \n", (int)arg);

    return NULL;
}

int treadpool_destory(threadpool_t *pool)
{
    int i;
    if(pool == NULL)
    {
        return -1;
    }
    pool->shutdown = true;

    //先销毁管理线程
    pthread_join(pool->adjust_tid, NULL);
    for(i = 0; i < pool->live_thr_num; i++)
    {
        //通知所有空闲线程
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }

    for(i = 0; i < pool->live_thr_num; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }

    threadpool_free(pool);
    return 0;
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
}