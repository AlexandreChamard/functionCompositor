
#include <sstream>

#include "evalexpr.hpp"

void test(std::string const &str) {
    auto res = Evalexpr::pExpr(str);

    std::cout << "test :\"" << str << "\"" << std::endl;
    std::cout << res << std::endl;
    if (res.valid()) {
        std::cout << "size = " << sizeof(res.f) << std::endl;
        std::cout << "exec : " << res() << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    std::stringstream sstr;

    srand(time(nullptr));

    sstr << rand() % 10;
    for (auto x = 0; x < 4; ++x) {
        int op = rand() % 5;
        switch (op) {
            case 0: sstr << "+"; break;
            case 1: sstr << "-"; break;
            case 2: sstr << "*"; break;
            case 3: sstr << "/"; break;
            case 4: sstr << "%"; break;
        }
        sstr << rand() % 10;
    }

    test(sstr.str());

    test("(34+12)*2-42");
    // test("-12");
    // test("+12");
    // test("+12a");
    // test("+");

}

// 75,840 bytes alloués
