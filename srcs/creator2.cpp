
#include <iostream>

#include "combinator.hpp"
#include "combinedFunction.hpp"

int main()
{
    auto a = Combinator::compile("");

    a->setReset(false);

    std::cout << a() << std::endl;
    std::cout << a() << std::endl;

    auto b = a;
    std::cout << b() << std::endl;
    std::cout << b() << std::endl;
    std::cout << a() << std::endl;
    std::cout << "size = " << sizeof(a) << std::endl;

    std::cout << "a->data = " << a->getData() << std::endl;
    std::cout << "&a->data = " << &a->getData() << std::endl;
}
