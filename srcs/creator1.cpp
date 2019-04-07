
#include <functional>
#include <iostream>
#include <tuple>
#include <memory>

/*
func that get nieme arg

func that add two func



*/

class Factory {
public:
    using vec_args = std::vector<int>;
    using vec_fargs = std::vector<std::function<int ()>>;

    std::shared_ptr<vec_args> args = std::make_shared<vec_args>();
    std::shared_ptr<vec_fargs> fargs = std::make_shared<vec_fargs>();

    void pushArg(int i) {
        args->emplace_back(i);
    }

    void genCallers(size_t n) {
        for (size_t i = fargs->size(); i <= n; ++i) {
            fargs->emplace_back([this, n] { return (*args)[n]; });
        }
    }

    auto getN(size_t n) {
        genCallers(n);
        return (*fargs)[n];
    }

    auto factoryAdd(std::function<int ()> const &a, std::function<int ()> const &b) {
        return [a, b]() {
            return a() + b();
        };
    }

    auto func(std::vector<int> const &rargs) {
        std::function<int ()> func = getN(0);
        for (size_t i = 1; i < rargs.size(); ++i) {
            func = factoryAdd(func, getN(i));
        }

        // return left;
        return [this, func] (std::vector<int> const &rargs) {
            for (auto &arg : rargs) {
                pushArg(arg);
            }

            return func();
        };
    }

};

class ExecFunction {

    std::function<int ()>  func;
    int                     data = 0;

public:
    int operator()() {
        return func();
    }

    int &getData() {return data;}

    void setFunc(std::function<int ()> &&f) {func = std::move(f);}
};

template<class Lambda, class Lambda2>
auto createAdd(int &i, Lambda &&a, Lambda2 &&b)
{
    return [&, a, b] () mutable {
        return i++ + a() + b();
    };
}

template<class Lambda, class Lambda2>
auto _createFunc(Lambda &&a, Lambda2 &&b)
{
    ExecFunction e;

    auto generatedFunc = createAdd(e.getData(), a, b);

    auto f = [&, generatedFunc] () mutable {
        return generatedFunc();
    };

    e.setFunc(f);

    return e;
}

auto bla()
{
    auto f1 = [] { return 1; };
    auto f2 = [] { return 42; };

    auto a = _createFunc(f1, f2);
    return (a);
}

int main()
{
    auto a = bla();
    std::cout << a() << std::endl;
    std::cout << a() << std::endl;
    a.getData() = 0;
    std::cout << a() << std::endl;
    std::cout << a() << std::endl;

    auto b = std::move(a);
    std::cout << b() << std::endl;
    std::cout << b() << std::endl;
    b.getData() = 0;
    std::cout << b() << std::endl;
    std::cout << b() << std::endl;
    std::cout << "size = " << sizeof(a) << std::endl;
}


// int main()
// {
//     // Factory factory;
//     // auto f = factory.func({1,2,3,4,5});

//     // std::cout << f({1,2,3,4,5}) << std::endl;


//     /*
//     fonction mere :
//     capture : 
//     - fonction fille
//     - object mutable

//     la fille doit avoir accès à l'object mutable de la mère par référence sans passage par argument
//     */


//     auto fille = [/* define b */] () mutable {
        
//     };

//     auto mere = [b = 0, fille]() mutable {
//         /* b = 0 */
//         fille();
//         /* b = 42 */
//     };

//     auto f1 = [] { return 1; };
//     auto f2 = [] { return 42; };

//     auto a = _createFunc(f1, f2);

//     std::cout << a() << std::endl;
//     std::cout << a() << std::endl;
//     std::cout << "size = " << sizeof(a) << std::endl;
// }

// /*
// pour générer une fonction mère :
// - avoir générer la/les fonctions filles
// - donner lors de la creation de la fonction mère les filles en copie dans la capture
// */