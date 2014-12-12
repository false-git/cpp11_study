#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <chrono>

class Hoge {
  public:
    Hoge() = default;
    virtual ~Hoge() = default;
    void set_callback(std::function<int(int)> func) {
	callback = func;
    }
    void start() {
	if (thread.joinable()) {
	    thread.join();
	}
	int c = 3;
	thread = std::thread([=] {
		std::this_thread::sleep_for(std::chrono::seconds(3));
		// ここでアクセスしているのは、コピーされたaではなく
		// コピーされたthisのa
		a = callback(5) + c;
		// cは、start()内のauto変数がキャプチャされている。
		// もし、コピーではなく参照にすると、ここを実行するときには
		// c は破棄されているので、結果は不定になる。
	    });
    }
    int wait_result() {
	if (thread.joinable()) {
	    thread.join();
	}
	return a;
    }
    void start2() {
	if (thread.joinable()) {
	    thread.join();
	}
	// スレッドの引数として、非staticなメンバ関数を渡すことができる。
	// このときは、第2引数にthisを渡す。
	// 第3引数の2は、runの第1引数になる。
	thread = std::thread(&Hoge::run, this, 2);
    }
    void run(int b) {
	std::this_thread::sleep_for(std::chrono::seconds(3));
	a = callback(5) + b;
    }
  private:
    int a;
    std::function<int(int)> callback;
    std::thread thread;
    Hoge(const Hoge&) = delete;
    Hoge(Hoge&&) = delete;
    Hoge& operator=(const Hoge&) = delete;
    Hoge& operator=(Hoge&&) = delete;
};

int main(int argc, char *argv[]) {
    int a = 5;
    // lambdaをfunction型の変数に初期値代入する。
    std::function<int()> f = [=]() -> int {
	return a * 2;
    };
    // lambdaを代入した変数を使って関数呼び出しをする。
    std::cout << f() << std::endl;
    // 簡単に書くとこう(引数リスト、返値の型は省略できる)
    auto f2 = [=] {
	return a * 2;
    };
    std::cout << f2() << std::endl;
    // キャプチャでコピーした変数はimmutableなので、以下はエラー
    /*
    auto f3 = [=] {
	a *= 2;
	return a;
    };
    */
    // mutableをつければok(mutableをつけるときは、引数リストが省略できない)
    // 返値の型は省略可能だが、位置を示すためにつけた。
    auto f3 = [=]() mutable -> int {
	a *= 2;
	return a;
    };
    // キャプチャでコピー指定しているので、当然mainのaは変わらない。
    std::cout << f3() << "," << a << std::endl;
    // キャプチャをコピーじゃなくて参照にする
    auto f4 = [&] {
	a *= 2;
	return a;
    };
    // キャプチャで参照指定しているので、mainのaが変わる。
    std::cout << f4() << "," << a << std::endl;

    // クラスの例
    Hoge hoge;
    hoge.set_callback([](int b) {
	    return b * 2;
	});
    hoge.start();
    std::cout << hoge.wait_result() << std::endl;
    hoge.start2();
    std::cout << hoge.wait_result() << std::endl;
    return 0;
}
