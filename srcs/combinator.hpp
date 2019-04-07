
#pragma once

#include "combinedFunction.hpp"

class Combinator {


protected:
    // à mettre dans une class Compiler
    template<class Lambda, class Lambda2>
    auto createAdd(int &i, Lambda &&a, Lambda2 &&b) {
        return [&, a, b] () mutable {
            return i++ + a() + b();
        };
    }

    // à mettre dans une class Compiler
    template<class Lambda, class Lambda2>
    auto createFunc(Lambda &&a, Lambda2 &&b) {
        function_ptr<CombinedFunction<int>> e = std::make_shared<CombinedFunction<int>>();

        auto generatedFunc = createAdd(e->getData(), a, b);

        auto f = [e, generatedFunc] () mutable {
            return generatedFunc();
        };

        e->setFunc(std::move(f));

        return e;
    }

public:

    static auto compile(std::string const &) {
        Combinator c;
        auto f1 = [] { return 1; };
        auto f2 = [] { return 42; };

        return c.createFunc(f1, f2);

    }

};
