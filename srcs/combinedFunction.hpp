
#pragma once

#include <functional>
#include <memory>
#include <mutex>

// this class is thread safe
// you can use it in parallel programing but it's not realy parallelism
template<class T>
class CombinedFunction {

    // function that was created
    std::function<int ()>   _func;

    bool _reset = true;
    std::mutex  mtx;

    // shared object
    T    _data;

private:
    struct locker {
        std::mutex &_m;
        locker(std::mutex &m): _m{m} { m.lock(); }
        ~locker() { _m.unlock(); }
    };

public:
    int operator()(void) {
        locker _(mtx);
        if (_reset)
            reset();
        return _func();
    }

    int &getData() { return _data; }

    void setReset(bool b) { _reset = b; }

    template<class Lambda>
    void setFunc(Lambda &&f) {
        reset();
        _func = std::move(f);
    }

protected:
    // this function reset the shared memory used by _func.
    void reset() {
        _data = T{};
    }
};

template<class T>
struct function_ptr : public std::shared_ptr<T> {
    function_ptr(std::shared_ptr<T> &&ptr): std::shared_ptr<T>{ptr} {}
    int operator()() {
        return std::shared_ptr<T>::get()->operator()();
    }
};
