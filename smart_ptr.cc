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
	std::cout << name_ << ": [DEAD] " << this << std::endl;
    }
    std::string name() const {
	return name_;
    }
  private:
    std::string name_;
};

void print(const std::string& name, const std::unique_ptr<Hoge>& ptr) {
    if (ptr) {
	std::cout << name << "(" << ptr->name() << "): " << static_cast<bool>(ptr) << ", " << ptr.get() << std::endl;
    } else {
	std::cout << name << "(): " << static_cast<bool>(ptr) << ", " << ptr.get() << std::endl;
    }
}

void print(const std::string& name, const std::shared_ptr<Hoge>& ptr) {
    std::cout << name << "(" << ptr->name() << "): " << ptr.use_count() << ", " << ptr.get() << std::endl;
}

int main(int argc, char *argv[]) {
    // auto変数。宣言してから、スコープを出るまで。
    Hoge one("one");
    std::cout << "========== step 1" << std::endl;
    {
	// unique_ptr。宣言してから、スコープを出るまで。
	std::unique_ptr<Hoge> two(new Hoge("two"));
	print("two", two);
	// コピー不可(以下はエラー)
	//std::unique_ptr<Hoge> three(two);
	//std::unique_ptr<Hoge> three = two;
	// 明示的にムーブ
	std::unique_ptr<Hoge> three(std::move(two));
	print("two", two);
	print("three", three);
	// resetで持ち直し。(元のがあればdelete)
	two.reset(new Hoge("two"));
	three.reset(new Hoge("three"));
	// swapで交換
	two.swap(three);
	print("two", two);
	print("three", three);
	// 関数に渡したり、返値で使う例にしないと、auto_ptrとあまり違わないかも
    }
    std::cout << "========== step 2" << std::endl;
    {
	// shared_ptr。宣言してから、全てのshared_ptrが消えるまで。
	std::shared_ptr<Hoge> four(new Hoge("four"));
	{
	    std::shared_ptr<Hoge> five(four);
	    print("four", four);
	    print("five", five);
	}
	print("four", four);
    }
    std::cout << "========== step 3" << std::endl;
    {
	// make_shared使った方が早いかつ安全って書いてあるんだけど、
	// 以下の書き方だとコピーが2回走る
	// →嘘だった。-std=c++11 をつけると、1回になった。
	std::shared_ptr<Hoge> four = std::make_shared<Hoge>("four");
    }
    std::cout << "========== step 4" << std::endl;
    {
	// 引数なしだと、デフォルトコンストラクタ1回しか呼ばれない。
	// →-std=c++11 をつけても、1回。
	std::shared_ptr<Hoge> noname = std::make_shared<Hoge>();
    }
    std::cout << "========== end" << std::endl;
    return 0;
}
