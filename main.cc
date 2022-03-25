#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

#include "ThreadSafeQueue.h"

ThreadSafeQueue<int> queue(10);

thread_local int counter = 0;

void producer() {
    std::cout << "producer \n";
    while (counter < 100) {
        queue.put(counter);
        std::cout << std::this_thread::get_id() << " put " << counter << std::endl;
        ++counter;
    }
}

thread_local int n = 110;

void consumer() {
    std::cout << "consumer \n";
    while (n > 0) {
        std::cout << std::this_thread::get_id() << " take " << queue.take() << std::endl;
        --n;
    }
}

void consumerWithTimeout() {
    std::cout << "consumer \n";
    while (n > 0) {
        int res = -1;
        if (queue.take(10, &res)) {
            std::cout << std::this_thread::get_id() << " take " << res << std::endl;
        } else {
            std::cout << "timeout\n";
        }
        --n;
    }
}

void test() {
    std::cout << "test\n";
}

int main(int argc, char** argv) {
    std::cout << "hello world\n";

    std::vector<std::thread> threads;

    for (int i = 0; i < 4; ++i) {
        if (i & 1) {
            threads.push_back(std::thread(producer));
        } else {
            threads.push_back(std::thread(consumerWithTimeout));
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // threads.push_back(std::thread(test));

    // threads.begin()->join();

    return 0;
}