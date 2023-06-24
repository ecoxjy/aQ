//这段代码使用了互斥锁来保护并发访问和更新数组S。每个worker重复10,000次操作，每次操作随机生成j，
//并更新S[j]的值为S[j]+S[(j+1)%N]+S[(j+2)%N]。通过使用细粒度的锁，可以降低冲突，提高并发度。
//同时，使用读锁和写锁的区别来保护读取和更新操作的原子性。
#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>

std::vector<int> S(100000); // 初始化长度为100,000的整数数组S
std::mutex mtx; // 创建互斥锁

int readerThread(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < 10000; i++) {
        int i1 = (i + 1) % 100000;
        int i2 = (i + 2) % 100000;

        //使用了一个std::vector<std::mutex>来表示每个元素的锁。
        std::lock_guard<std::mutex> lock(mtx);
        int s_i = S[i];
        int s_i1 = S[i1]; 
        int s_i2 = S[i2];
        return s_i + s_i1 + s_i2;
    }
}

void writerThread(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 99999);
    int j = dis(gen);
    S[j] = readerThread(id);
}
/* void worker(int id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 99999);

    for (int i = 0; i < 10000; i++) {
        int j = dis(gen);
        int i1 = (i + 1) % 100000;
        int i2 = (i + 2) % 100000;

        //使用了一个std::vector<std::mutex>来表示每个元素的锁。
        //在读写元素时，使用std::lock_guard来自动获取对应元素的锁，并返回和更新元素的值。
        // std::lock_guard对象构造时，自动调用mtx.lock()进行上锁
        // std::lock_guard对象析构时，自动调用mtx.unlock()释放锁
        //读取S(i), S(i+1), S(i+2)的值
        std::lock_guard<std::mutex> lock(mtx);
        int s_i = S[i];
        int s_i1 = S[i1]; 
        int s_i2 = S[i2];

        // 写S(j)
        S[j] = s_i + s_i1 + s_i2;
    }
} */

int main() {
    int M = 5; // workers数量
    std::vector<std::thread> threads;

    for (int i = 0; i < M; i++) {
        threads.emplace_back(readerThread, i);
        threads.emplace_back(writerThread, i);
    }

    for (auto& thread : threads) {
        thread.join();//保证子线程的完成
    }

    return 0;
}