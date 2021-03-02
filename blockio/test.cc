 #include <iostream>
 #include <thread>
 #include <chrono>
 #include <vector>
 #include <mutex>
  
 class Task{
 public:
     Task(int x, std::shared_ptr<std::mutex> mutex)
             :_x(x), _mutex(mutex){ }
     
     void handle(){
         int task_id = 0;
         while(true){
             //获取任务, 尽早释放锁
             if (_x > 0){ 
                 std::lock_guard<std::mutex> lock(*_mutex);
                 if (_x > 0){ task_id = _x; --_x; }
                 else { _x = 0; }
             }   
             else { return ; } 
 
             //do task
             std::cout << "do task id: " << task_id << std::endl;
             std::this_thread::sleep_for(std::chrono::seconds(1));
         }   
     }   
 private:
     int _x; 
     std::shared_ptr<std::mutex> _mutex;
 };
 
 int main()
 {
     int x = 0;
     const int THREAD_NUM = 7;
     const int TASK_NUM = 100;
     std::vector<std::thread> threads;
 
     //shared_ptr 主线程与子线程可以共用一把锁.
     //方便后面扩展程序(生产/消费者)
     std::shared_ptr<std::mutex> mutex = std::make_shared<std::mutex>();
     Task t(TASK_NUM, mutex);
 
     //新建线程, std::thread支持使用lambda
     for (int i = 0; i < THREAD_NUM; ++i){
         threads.emplace_back(std::thread(
             [&t] { t.handle(); })
         );
     }
 
     //等待线程结束
     for(auto &thread : threads){ thread.join(); }
     return 0;
 }