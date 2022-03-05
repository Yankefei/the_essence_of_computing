#ifndef _TOOLS_SINGLETON_H_
#define _TOOLS_SINGLETON_H_

#include <type_traits>
#include <map>
#include <mutex>
#include <tuple>

namespace tools
{

template<typename T>
class Singleton
{
public:
    // 获取无构造参数类的单利对象指针
    static inline T* GetInstance();
    
    // 获取有对应一组构造参数的单利对象指针
    /*
        类C的构造函数形式为：C(int a, double b)
        每次调用GetInstance<C>(0, 0.1)会返回同一个C类的指针
        调用GetInstance<C>(0, 0.1), GetInstance<C>(0, 0.2), GetInstance<C>(1, 0.1)将分别返回三个不同的指针
    */
    template <typename... Types>
    static T* GetInstance(Types&&... args);
};

template <typename T>
T* Singleton<T>::GetInstance()
{
    static T t;
    return &t;
}

template <typename T>
template <typename... Types>
T* Singleton<T>::GetInstance(Types&&... args)
{
    typedef std::tuple
        <typename std::remove_reference<Types>::type...> TupleType;  // 使用形参类型为模板参数的tuple
        
    static std::map<TupleType, T*> ins_map;
    static std::mutex mtx;
    const auto& key = std::make_tuple(std::forward<Types>(args)...);
    std::unique_lock<std::mutex> lock(mtx);
    auto it = ins_map.find(key);
    if (it != ins_map.end())
    { return it->second; }
    
    T* new_t = new T(std::forward<Types>(args)...);
    ins_map[key] = new_t;
    return new_t;
}

}

#endif