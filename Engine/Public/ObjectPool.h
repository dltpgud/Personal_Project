#pragma once
#include "typeindex"
#include "typeinfo"
#include  "Engine_Defines.h"

template <typename Base> 
class ObjectPool
{
public:
    template <typename T, typename... Args> static T* Pop(T* prototype, Args&&... args)
    {
        auto& pool = s_pools[type_index(typeid(*prototype))];

        if (!pool.empty())
        {
            void* raw = pool.back();
            pool.pop_back();
    
            T* obj = static_cast<T*>(raw);
            obj->Initialize(std::forward<Args>(args)...);
            return obj;
        }

        return prototype->Clone(std::forward<Args>(args)...);
    }

    template <typename T> static void Push(T* obj)
    {
        if (!obj)
            return;
        

        auto& pool = s_pools[type_index(typeid(*obj))];
        Base* base = static_cast<Base*>(obj); //  lvalue
        Safe_AddRef(base);  
        pool.push_back(static_cast<void*>(obj));
    }

    static void ClearAll()
    {

        // 1) 먼저 포인터들을 다 뽑아놓고 컨테이너를 비움
         vector<void*> toRelease;
        for (auto& pair : s_pools)
        {
            auto& vec = pair.second;
            toRelease.insert(toRelease.end(), vec.begin(), vec.end());
            vec.clear();
        }

        // 2) 그 다음에 Release (이때 Push가 일어나도 vec는 이미 비어있음)
        for (void* raw : toRelease)
        {
            Base* base = static_cast<Base*>(raw);
            Safe_Release(base);
        }
     
    }

    template <typename T, typename... Args> static void Preallocate(T* prototype, size_t count, Args&&... args)
    {
        auto& pool = s_pools[type_index(typeid(*prototype))];

        for (size_t i = 0; i < count; ++i)
        {
            T* obj = prototype->Clone(forward<Args>(args)...);
            Push(obj);

            // 여기서 1개 내려서 ref=1 (풀만 소유)
            Base* base = static_cast<Base*>(obj);
            Safe_Release(base);
   
        }
    }

private:
    static unordered_map<type_index, vector<void*>> s_pools;

};

