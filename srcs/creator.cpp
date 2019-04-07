
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <functional>
#include <algorithm>

#include <tuple>
#include <utility>
#include <type_traits>
#include <stdexcept>


template<
  typename Tuple,
  typename Indices=std::make_index_sequence<std::tuple_size<Tuple>::value>>
struct runtime_get_func_table;

template<typename Tuple,size_t ... Indices>
struct runtime_get_func_table<Tuple,std::index_sequence<Indices...>>{
    using return_type=typename std::tuple_element<0,Tuple>::type &;
    using get_func_ptr=return_type (*)(Tuple &) noexcept;
    static constexpr get_func_ptr table[std::tuple_size<Tuple>::value] = {
        &std::get<Indices>...
    };
};

template<typename Tuple,size_t ... Indices>
constexpr typename
runtime_get_func_table<Tuple,std::index_sequence<Indices...>>::get_func_ptr
runtime_get_func_table<Tuple,std::index_sequence<Indices...>>::table[std::tuple_size<Tuple>::value];

template<typename Tuple>
constexpr
typename std::tuple_element<0,typename std::remove_reference<Tuple>::type>::type&
runtime_get(Tuple&& t,size_t index){
    using tuple_type=typename std::remove_reference<Tuple>::type;
    if(index>=std::tuple_size<tuple_type>::value)
        throw std::runtime_error("Out of range");
    return runtime_get_func_table<tuple_type>::table[index](t);
}


/*
la fonction compute fait :
- prend un chaine de charactère en paramètre
- compile l'expression pour créer un fonction permettant d'évalué cette expression.
- Genère une fonction prenant en paramètre les variables dans l'ordre alphanumérique.
- une variable à pour format : [a-zA-Z_][a-zA-Z0-9_]*
*/


template<class Res, class ...Args>
class Creator;

template<class Res, class ...Args>
class Creator<Res (Args...)> {
    using arg_type = std::function<int ()>;

protected:
    std::vector<std::string> tokens;
    std::map<std::string, std::function<int (int, int)>> operators;
    std::vector<std::string> vars;

public:
    static std::function<Res (std::tuple<Args...>)> compute(std::string const &str) {
        return Creator<Res (Args...)>{}._compute(str);
    }

private:

    Creator() {
        operators.emplace("+", [](int i, int j){return i+j;});
        operators.emplace("-", [](int i, int j){return i-j;});
    }

    std::function<Res (std::tuple<Args...>)> _compute(std::string const &str) {
        tokenize(str);
        
        for (auto &e : tokens) {
            std::cout << e << std::endl;
        }

        auto it = tokens.begin();

        auto f = getNextFunc(it, tokens.end());

        if (f == nullptr)
            return [](std::tuple<Args...> &&args){return -1;};

        // while (auto f2 = getNextFunc(tokens.begin(), tokens.end())) {

        //     f = f2;
        // }
        return [f](std::tuple<Args...> &&args){
            return (*f)(runtime_get(args, 0), runtime_get(args, 1));
        };
    }

    void tokenize(std::string const &str) {
        // very bad tokenizer !
        for (auto const &c : str) {
            tokens.emplace_back(std::string()+c);
        }

        // puts all vars in a vector and sort them
        for (auto &t : tokens) {
            if (operators.find(t) == operators.end() && std::find(vars.begin(), vars.end(), t) != vars.end()) {
                vars.emplace_back(t);
            }
        }
        std::sort(vars.begin(), vars.end());
    }

    std::size_t getVarPos(std::string const &varName) {
        auto it = std::find(vars.begin(), vars.end(), varName);

        if (it == vars.end())
            throw std::runtime_error{};
        return it - vars.begin();
    }

    bool isVariable(std::string const &str) {
        return std::find(vars.begin(), vars.end(), str) != vars.end();
    }

    bool isOperator(std::string const &str) {
        return operators.find(str) != operators.end();
    }

    std::function<int (int, int)> *getNextFunc(std::vector<std::string>::iterator it, std::vector<std::string>::iterator const &end) {

        while (it != end && !isOperator(*it)) {
            ++it;
        }
        if (it == end)
            return nullptr;
        return &operators[*it];
    };

};

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cerr << "USAGE : "<<*av<<" expression" << std::endl;
        return 1;
    }
    auto func =  Creator<int (int, int)>::compute(av[1]);

    std::cout << func(std::make_tuple(42, 1)) << std::endl;
}