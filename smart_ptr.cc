#include <iostream>
#include <string>
#include <vector>

static bool s_nodebug;

class Hoge {
  public:
    Hoge() {
	if (!s_nodebug) {
	    std::cout << name_ << ": [BORN] (default) " << this << std::endl;
	}
    }
    Hoge(const Hoge& hoge): name_(hoge.name_) {
	if (!s_nodebug) {
	    std::cout << name_ << ": [BORN] (copy) " << this << std::endl;
	}
    }
    Hoge(const std::string& name): name_(name) {
	if (!s_nodebug) {
	    std::cout << name_ << ": [BORN] " << this << std::endl;
	}
    }
    virtual ~Hoge() {
	if (!s_nodebug) {
	    std::cout << name_ << ": [DEAD] " << this << std::endl;
	}
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

void print(const std::string& name, const std::weak_ptr<Hoge>& ptr) {
    if (std::shared_ptr<Hoge> sp = ptr.lock()) {
	print(name, sp);
    } else {
	std::cout << name << "(): " << ptr.use_count() << ", " << ptr.expired() << std::endl;
    }
}

int main(int argc, char *argv[]) {
    // auto変数。宣言してから、スコープを出るまで。
    Hoge one("one");
    std::cout << "========== step 1 unique_ptr" << std::endl;
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
    std::cout << "========== step 2 shared_ptr" << std::endl;
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
    std::cout << "========== step 3 make_shared(with args)" << std::endl;
    {
	// make_shared使った方が早いかつ安全って書いてあるんだけど、
	// 以下の書き方だとコピーが2回走る
	// →嘘だった。-std=c++11 をつけると、1回になった。
	std::shared_ptr<Hoge> four = std::make_shared<Hoge>("four");
    }
    std::cout << "========== step 4 make_shared(defalut)" << std::endl;
    {
	// 引数なしだと、デフォルトコンストラクタ1回しか呼ばれない。
	// →-std=c++11 をつけても、1回。
	std::shared_ptr<Hoge> noname = std::make_shared<Hoge>();
    }
    std::cout << "========== step 5 weak_ptr" << std::endl;
    {
	// weak_ptr。shared_ptrとセットで使う。
	// lock()を呼び出してshared_ptrを取得しないと使えない。
	// shared_ptrを使うと循環参照になるような場合に使う。
	std::weak_ptr<Hoge> six;
	print("six", six);
	{
	    std::shared_ptr<Hoge> seven = std::make_shared<Hoge>("seven");
	    six = seven;
	    print("six", six); // print の中でlockするので、参照カウントが2になる。
	    print("seven", seven);
	}
	print("six", six);
	// make_sharedを使うと、内側のブロックを抜けた時点でHogeのデストラクタ
	// は呼ばれるが、管理領域が解放されない。外側のブロックを抜けた時点で
	// 解放される、みたいな記述あり。
	// weak_countみたいなものを取る手段はないみたいだけど。
    }
    std::cout << "========== step 6 more unique_ptr" << std::endl;
    {
	std::vector<std::unique_ptr<Hoge>> vec;
	vec.push_back(std::unique_ptr<Hoge>(new Hoge("eight")));
	std::unique_ptr<Hoge> nine(new Hoge("nine"));
	vec.push_back(std::move(nine));
	print("nine", nine);
	std::vector<std::unique_ptr<Hoge>>::iterator it;
	for (it = vec.begin(); it != vec.end(); it++) {
	    // vectorから取り出さなければ、普通にiteratorで回して良いらしい。
	    print((*it)->name(), *it);
	}
	std::vector<std::unique_ptr<Hoge>> vec2;
	// コピーしようとするとエラー。
	//vec2 = vec;
	// moveなら大丈夫。
	for (it = vec.begin(); it != vec.end(); it++) {
	    vec2.push_back(std::move(*it));
	}
	int i = 0;
	for (it = vec.begin(); it != vec.end(); it++) {
	    // moveしたので全部空になっている。
	    std::cout << i++ << ": " << static_cast<bool>(*it) << std::endl;
	}
	for (it = vec2.begin(); it != vec2.end(); ) {
	    print((*it)->name(), *it);
	    it = vec2.erase(it); // このタイミングでunique_ptrの寿命がつきる
	}
    }
    std::cout << "========== step 7 more more unique_ptr" << std::endl;
    s_nodebug = true;
    {
	std::vector<std::unique_ptr<Hoge>> vec;
	for (int i = 0; i < 1024; i++) {
	    vec.push_back(std::unique_ptr<Hoge>(new Hoge()));
	    // 以下を出力すると、倍々で拡張しているよう
	    // 拡張が発生するとコピーが走るはずだが、特にエラーにならない。
	    //std::cout << vec.capacity() << std::endl;
	}
	std::vector<std::unique_ptr<Hoge>> vec2;
	// この方法でもmoveも可能
	vec2 = std::move(vec);
	std::cout << vec.size() << ", " << vec2.size() << std::endl;
    }
    s_nodebug = false;
    std::cout << "========== end" << std::endl;
    return 0;
}
