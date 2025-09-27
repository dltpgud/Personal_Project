#pragma once
#include "Engine_Defines.h"

template <typename Type> class ObjectPool
{
public:
    template <typename... Args> static Type* Pop(const Type& prototype, Args&&... args)
    {
        //std::lock_guard<std::mutex> lock(s_mutex);

        if (!s_pool.empty())
        {
            Type* obj = s_pool.back();
            s_pool.pop_back();
          
            // 생성자 호출 X → 직접 Reset 필요
                obj->Initialize_PoolObj(std::forward<Args>(args)...);

            return obj;
        }
        else
        {
            Type* obj = new Type(prototype);
            obj->Initialize(std::forward<Args>(args)...);
            return obj;
        }
    }

    static void Push(Type* obj)
    {
        if (!obj)
            return;

        // 소멸자 호출하지 않음
       // std::lock_guard<std::mutex> lock(s_mutex);
        s_pool.push_back(obj);
    }

    static void ClearPool()
    {
       // std::lock_guard<std::mutex> lock(s_mutex);
        for (Type* obj : s_pool) delete (obj); // 여기서는 정상 delete (소멸자 호출)
        s_pool.clear();
    }

private:
    static std::vector<Type*> s_pool;
   // static std::mutex s_mutex;
};

template <typename Type> std::vector<Type*> ObjectPool<Type>::s_pool;

//template <typename Type> std::mutex ObjectPool<Type>::s_mutex;
