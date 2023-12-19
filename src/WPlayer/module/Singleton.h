/*****************************************************************//**
 * \file   Singleton.h
 * \brief  单例类模板
 * 
 * \author wuguochang
 * \date   2023/12/11
 * \note History record:
 * 
 *********************************************************************/
#pragma once
 // 声明友元
#define MAKE_SINGLETON_FRIEND(T)        friend class Singleton<T>;
// 禁用拷贝
#define DELETE_COPY_FUNCTION(T)         T(const T&) = delete; \
                                        const T& operator= (const T&) = delete;
// 快捷单例配置
#define SINGLETON_CONFIG(T)             MAKE_SINGLETON_FRIEND(T)\
                                        DELETE_COPY_FUNCTION(T) \
                                        private: \
                                        T(QObject* parent = nullptr);\
                                        ~T(); 



#if 1
// c++11以上版本支持
template<typename T>
class Singleton
{
public:
    inline static T& getInstance()
    {
        static T instance;
        return instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

#else
// 传统双检查锁单例类模板
#include <mutex>

template <typename T>
class Singleton {
public:
    static T& getInstance() {
        if (instance_ == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_); // 使用互斥锁保护多线程访问
            if (instance_ == nullptr) {
                instance_ = new T();
            }
        }
        return *instance_;
    }

    // 删除复制构造函数和赋值操作符，以防止复制实例
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() {} // 保护构造函数，派生类可以访问
    virtual ~Singleton() {} // 虚析构函数确保正确的销毁

private:
    static T* instance_; // 单例实例指针
    static std::mutex mutex_; // 互斥锁，保护对单例的访问
};

// 初始化静态成员
template <typename T>
T* Singleton<T>::instance_ = nullptr;

template <typename T>
std::mutex Singleton<T>::mutex_;
#endif
