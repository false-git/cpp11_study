#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

// mutexの他に、recursive_mutex, timed_mutex, recursive_timed_mutexがある。
std::mutex log_mutex;
std::mutex s_mutex;
// condition_variableの他に、condition_variable_anyがある。
std::condition_variable condition;
bool finished;

int func1(int arg) {
    {
	std::lock_guard<std::mutex> lock(log_mutex);
	std::cout << "func1(" << arg << "): in" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(arg));
    {
	// lock_guardは変数の寿命の間mutexをlockする。
	std::lock_guard<std::mutex> lock(s_mutex);
	finished = true;
	condition.notify_all();
    }
    {
	std::lock_guard<std::mutex> lock(log_mutex);
	std::cout << "func1(" << arg << "): notify" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    {
	std::lock_guard<std::mutex> lock(log_mutex);
	std::cout << "func1(" << arg << "): out" << std::endl;
    }
    return arg * 2; // 返値は捨てられる
}

void func2() {
    {
	std::lock_guard<std::mutex> lock(log_mutex);
	std::cout << "func2(): in" << std::endl;
    }
    {
	// unique_lockは、lock_guardの機能に加えて、lock(), unlock()ができる
	std::unique_lock<std::mutex> lock(s_mutex);
	condition.wait(lock, [&]{
		std::lock_guard<std::mutex> lock(log_mutex);
		std::cout << "wait(" << finished << ")" << std::endl;
		return finished;
	    });
	// notifyを送る側が確実にこちらのスレッドよりも先にmutexをlockできて、
	// 単に終了待ちをしたいだけならば1引数版のwait()を使っても良い。
	// そうでない場合は、この例のようにwaitを抜けても良いかどうかを判断
	// する関数を渡す。
	// タイムアウト付きで待ちたい場合は、wait_for(), wait_until()を使う
    }
    {
	std::lock_guard<std::mutex> lock(log_mutex);
	std::cout << "func2(): out" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    std::thread t(func1, 10);
    t.detach();
    std::thread t2(func2);
    {
	std::lock_guard<std::mutex> lock(log_mutex);
	std::cout << "main(): before join" << std::endl;
    }
    // join() またはdetach()しないでthreadのデストラクタが呼ばれると、
    // std::terminate()が呼ばれる。
    t2.join();
    {
	std::lock_guard<std::mutex> lock(log_mutex);
	std::cout << "main(): after join" << std::endl;
    }
    // detachしたfunc1の終了を待たずにmainが終わると、func1も強制的に終わるらしい。
    return 0;
}
