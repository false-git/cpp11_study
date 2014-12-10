#include <iostream>
#include <string>

class Hoge {
  public:
    Hoge() {
	std::cout << name_ << ": [BORN] (default) " << this << std::endl;
    }
    Hoge(const Hoge& hoge): name_(hoge.name_) {
	std::cout << name_ << ": [BORN] (copy) " << this << std::endl;
    }
    Hoge(const std::string& name): name_(name) {
	std::cout << name_ << ": [BORN] " << this << std::endl;
    }
    virtual ~Hoge() {
	std::cout << name_ << ": [DEAD]" << std::endl;
    }
  private:
    std::string name_;
};

int main(int argc, char *argv[]) {
    // auto変数。宣言してから、スコープを出るまで。
    Hoge one("one");
    std::cout << "========== step 1" << std::endl;
    {
	// unique_ptr。宣言してから、スコープを出るまで。
	std::unique_ptr<Hoge> two(new Hoge("two"));
	std::cout << "two: " << two << ", " << two.get() << std::endl;
	// コピー不可(以下はエラー)
	//std::unique_ptr<Hoge> three(two);
	//std::unique_ptr<Hoge> three = two;
	// 明示的にムーブ
	std::unique_ptr<Hoge> three(std::move(two));
	std::cout << "two: " << two << ", " << two.get() << std::endl;
	std::cout << "three: " << three << ", " << three.get() << std::endl;
	// resetで持ち直し。(元のがあればdelete)
	two.reset(new Hoge("two"));
	three.reset(new Hoge("three"));
	// swapで交換
	two.swap(three);
	std::cout << "two: " << two << ", " << two.get() << std::endl;
	std::cout << "three: " << three << ", " << three.get() << std::endl;
	// 関数に渡したり、返値で使う例にしないと、auto_ptrとあまり違わないかも
    }
    std::cout << "========== step 2" << std::endl;
    {
	// shared_ptr。宣言してから、全てのshared_ptrが消えるまで。
	std::shared_ptr<Hoge> four(new Hoge("four"));
	{
	    std::shared_ptr<Hoge> five(four);
	    std::cout << "four: " << four.use_count() << ", " << four.get() << std::endl;
	    std::cout << "five: " << five.use_count() << ", " << five.get() << std::endl;
	}
	std::cout << "four: " << four.use_count() << ", " << four.get() << std::endl;
    }
    std::cout << "========== step 3" << std::endl;
    {
	// make_shared使った方が早いかつ安全って書いてあるんだけど、
	// 以下の書き方だとコピーが2回走る
	std::shared_ptr<Hoge> four = std::make_shared<Hoge>("four");
    }
    std::cout << "========== end" << std::endl;
    return 0;
}
