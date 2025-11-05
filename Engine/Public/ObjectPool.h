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
        pool.push_back(static_cast<void*>(obj));
    }

    static void ClearAll()
    {
        for (auto& pair : s_pools)
        {
            for (auto* raw : pair.second)
            {
                Base* obj = static_cast<Base*>(raw);
                Safe_Release(obj);
            }
            pair.second.clear();
        }
        s_pools.clear();
    }

    template <typename T> static void Clear()
    {
        auto iter = s_pools.find(type_index(typeid(T)));
        if (iter != s_pools.end())
        {
            for (auto* raw : iter->second)
            {
                Base* obj = static_cast<Base*>(raw);
                Safe_Release(obj);
            }
            iter->second.clear();
            s_pools.erase(iter);
        }
    }

    template <typename T, typename... Args> static void Preallocate(T* prototype, size_t count, Args&&... args)
    {
        auto& pool = s_pools[type_index(typeid(*prototype))];

        for (size_t i = 0; i < count; ++i)
        {
            T* obj = prototype->Clone(forward<Args>(args)...);
            pool.push_back(static_cast<void*>(obj));
        }
    }

private:
    static unordered_map<type_index, vector<void*>> s_pools;
};

