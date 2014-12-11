#include <iostream>
#include <string>
#include <vector>

static bool s_nodebug;

class Hoge {
  public:
    Hoge() {
	if (!s_nodebug) {
	    std::cout << "Hoge(" << this << "): " << name_ << ": [BORN] (default)" << std::endl;
	}
    }
    Hoge(const Hoge& hoge): name_(hoge.name_) {
	if (!s_nodebug) {
	    std::cout << "Hoge(" << this << "): " << name_ << ": [BORN] (copy)" << std::endl;
	}
    }
    Hoge(Hoge&& hoge): name_(hoge.name_) {
	hoge.name_ = "";
	if (!s_nodebug) {
	    std::cout << "Hoge(" << this << "): " << name_ << ": [BORN] (move)" << std::endl;
	}
    }
    Hoge(const std::string& name): name_(name) {
	if (!s_nodebug) {
	    std::cout << "Hoge(" << this << "): " << name_ << ": [BORN] (with args)" << std::endl;
	}
    }
    virtual ~Hoge() {
	if (!s_nodebug) {
	    std::cout << "Hoge(" << this << "): " << name_ << ": [DEAD]" << std::endl;
	}
    }
    Hoge& operator=(Hoge& hoge) {
	name_ = hoge.name_;
	if (!s_nodebug) {
	    std::cout << "Hoge(" << this << "): " << name_ << ": (copy)" << std::endl;
	}
	return *this;
    }
    Hoge& operator=(Hoge&& hoge) {
	name_ = hoge.name_;
	hoge.name_ = "";
	if (!s_nodebug) {
	    std::cout << "Hoge(" << this << "): " << name_ << ": (move)" << std::endl;
	}
	return *this;
    }
    std::string name() const {
	return name_;
    }
  private:
    std::string name_;
};

class Moge {
  public:
    Moge() {
	if (!s_nodebug) {
	    std::cout << "Moge(" << this << "): " << name_ << ": [BORN] (default)" << std::endl;
	}
    }
    Moge(const Moge& hoge): name_(hoge.name_) {
	if (!s_nodebug) {
	    std::cout << "Moge(" << this << "): " << name_ << ": [BORN] (copy)" << std::endl;
	}
    }
    Moge(const std::string& name): name_(name) {
	if (!s_nodebug) {
	    std::cout << "Moge(" << this << "): " << name_ << ": [BORN] (with args)" << std::endl;
	}
    }
    virtual ~Moge() {
	if (!s_nodebug) {
	    std::cout << "Moge(" << this << "): " << name_ << ": [DEAD]" << std::endl;
	}
    }
    /*
      copy用のoperator=だけがあって、move用のoperator=がないと、関数の
      返値を代入できなくなる。 
    Moge& operator=(Moge& hoge) {
	name_ = hoge.name_;
	if (!s_nodebug) {
	    std::cout << "Moge(" << this << "): " << name_ << ": (copy)" << std::endl;
	}
	return *this;
    }
    */
    std::string name() const {
	return name_;
    }
  private:
    std::string name_;
};

void print(const std::string name, const Hoge& hoge) {
    std::cout << name << ": " << hoge.name() << ": " << &hoge << std::endl;
}

void print(const std::string name, const Moge& moge) {
    std::cout << name << ": " << moge.name() << ": " << &moge << std::endl;
}

void func1(Hoge hoge) {
    print("func1", hoge);
}

Hoge func2() {
    return Hoge("func2");
}

Moge func3() {
    return Moge("func3");
}

int main(int argc, char *argv[]) {
    std::cout << "========== step 1 copy" << std::endl;
    {
	Hoge a("a"); // constructor with args
	Hoge b(a);   // copy constructor
	Hoge c;      // default constructor
	print("a", a);
	print("b", b);
	print("c", c);
	c = a;       // operator=(copy)
	print("c", c);
    }
    std::cout << "========== step 2 move" << std::endl;
    {
	Hoge a("a"); // constructor with args
	print("a", a);
	Hoge b(std::move(a)); // move constructor
	print("a", a);
	print("b", b);
	Hoge c;      // default constructor
	print("c", c);
	c = std::move(b);       // operator=(move)
	print("a", a);
	print("b", b);
	print("c", c);
    }
    std::cout << "========== step 3 argument" << std::endl;
    {
	Hoge a("a"); // constructor with args
	func1(a); // コピーコンストラクタが呼ばれる
	func1(std::move(a)); // moveコンストラクタが呼ばれる
	print("old-a", a);
    }
    std::cout << "========== step 4 return value" << std::endl;
    {
	Hoge a = func2();
	Hoge b;
	b = func2(); // operator=(move)
	Moge c = func3();
	print("c", c);
	Moge d;
	d = func3(); // デフォルトのoperator=の呼び出し
	print("d", d);
    }
    std::cout << "========== end" << std::endl;
    return 0;
}
