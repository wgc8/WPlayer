/*****************************************************************//**
 * \file   Singleton.h
 * \brief  ������ģ��
 * 
 * \author wuguochang
 * \date   2023/12/11
 * \note History record:
 * 
 *********************************************************************/
#pragma once
 // ������Ԫ
#define MAKE_SINGLETON_FRIEND(T)        friend class Singleton<T>;
// ���ÿ���
#define DELETE_COPY_FUNCTION(T)         T(const T&) = delete; \
                                        const T& operator= (const T&) = delete;
// ��ݵ�������
#define SINGLETON_CONFIG(T)             MAKE_SINGLETON_FRIEND(T)\
                                        DELETE_COPY_FUNCTION(T) \
                                        private: \
                                        T(QObject* parent = nullptr);\
                                        ~T(); 



#if 1
// c++11���ϰ汾֧��
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
// ��ͳ˫�����������ģ��
#include <mutex>

template <typename T>
class Singleton {
public:
    static T& getInstance() {
        if (instance_ == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_); // ʹ�û������������̷߳���
            if (instance_ == nullptr) {
                instance_ = new T();
            }
        }
        return *instance_;
    }

    // ɾ�����ƹ��캯���͸�ֵ���������Է�ֹ����ʵ��
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() {} // �������캯������������Է���
    virtual ~Singleton() {} // ����������ȷ����ȷ������

private:
    static T* instance_; // ����ʵ��ָ��
    static std::mutex mutex_; // �������������Ե����ķ���
};

// ��ʼ����̬��Ա
template <typename T>
T* Singleton<T>::instance_ = nullptr;

template <typename T>
std::mutex Singleton<T>::mutex_;
#endif
