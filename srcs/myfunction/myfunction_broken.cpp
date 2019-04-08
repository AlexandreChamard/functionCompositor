
#include <string>
#include <string.h>
#include <iostream>
#include <tuple>
#include <functional>

///
/// myfunction fonctionne. seulement elle ne détruit pas la catpure si l'objet est un lambda ou tout autre objet invocable.
///

template<class Signature>
class myfunction;

template<class Ret, class ...Args>
class myfunction<Ret (Args...)> {

    bool isObject = true;
    union {
        Ret (*p)(Args...);
        struct {
            void *obj;
            void *ptr;
            // void (*dtor)(void *);
        };
    };

public:
    template<class T, typename std::enable_if_t<std::conjunction_v<std::is_invocable<T, Args...>, std::is_class<T>>, int> = 0>
    myfunction(T &&t): isObject{true}
    {
        union { decltype(&T::operator()) f; void *p_ope; } caster{&T::operator()};
        // union { decltype(&T::~T) f; void (*p_dtor)(void *); } caster2{&T::~T};

        // union { decltype(&decltype(t)::operator()) f; void *p_ope; } caster{&decltype(t)::operator()};
        ptr = caster.p_ope;
        // dtor = caster2.p_dtor;

        obj = new char[sizeof(T)];
        new ((T*)obj) T(std::move(t));
        // memcpy(obj, &t, sizeof(T));
    }

    template<class T, typename std::enable_if_t<std::conjunction_v<std::is_invocable<T, Args...>, std::negation<std::is_class<T>>>, int> = 0>
    myfunction(T &&t): isObject{false}, p{t}
    {
        std::cout << "function ctor" << std::endl;
    }

    ~myfunction() {
        if (isObject == true) {
            delete[] (char *)obj;
            // need to call T::~T(obj)
        }
    }

    Ret operator()(Args &&...args) {
        if (isObject) {
            return reinterpret_cast<Ret (*)(void *, Args...)>(ptr)(obj, args...);
        } else {
            return p(args...);
        }
    }
    // template<class T, typename std::enable_if_t<!std::is_invocable_v<T>, int> = 0>
    // myfunction(T &&t) {
    //     std::cout << "error" << std::endl;
    // }
};

struct Bla {

    char *bla;

    Bla() {
        bla = new char;
        std::cout << "Bla ctor" << std::endl;
    }
    Bla(Bla const &_b) {
        std::cout << "Bla cpy" << std::endl;
        bla = new char;
    }
    Bla(Bla &&_b) {
        std::cout << "Bla move" << std::endl;
        bla = _b.bla;
        _b.bla = nullptr;
    }
    ~Bla() {
        std::cout << "Bla dtor" << std::endl;
        delete bla;
    }
};

int main()
{
    myfunction<bool ()> f1([b = Bla()]() {return false;});
    // myfunction<bool ()> f3([](){std::cout << "lambda" << std::endl; return true;});

    auto a = []() {};

    // f3();

    f1();

}