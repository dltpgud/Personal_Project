#pragma once
#include "typeindex"
#include "typeinfo"
#include  "Engine_Defines.h"

template <typename Base> 
class ObjectPool
{
public:
    template <typename T, typename... Args> static T* Pop(T& prototype, Args&&... args)
    {
        auto& pool = s_pools[type_index(typeid(prototype))];
       // std::cout << "[Pop] key = " << typeid(prototype).name() << " pool size = " << pool.size() << std::endl;

        if (!pool.empty())
        {
            void* raw = pool.back();
            pool.pop_back();

            T* obj = static_cast<T*>(raw);
         //   std::cout << "  -> Pop return: " << typeid(*obj).name() << std::endl;
            obj->Initialize(std::forward<Args>(args)...);
            return obj;
        }

//        std::cout << "  -> Clone new: " << typeid(prototype).name() << std::endl;
        return prototype.Clone(std::forward<Args>(args)...);
    }

    template <typename T> static void Push(T* obj)
    {
        if (!obj)
            return;
        if (auto* derived = dynamic_cast<T*>(obj))
        {
            auto& pool = s_pools[type_index(typeid(T))];
            pool.push_back(static_cast<void*>(derived));
  //          std::cout << "[Push] key = " << typeid(T).name() << " pool size = " << pool.size() << std::endl;
        }
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

    template <typename T, typename... Args> static void Preallocate(T& prototype, size_t count, Args&&... args)
    {
        auto& pool = s_pools[type_index(typeid(prototype))];

        for (size_t i = 0; i < count; ++i)
        {
            T* obj = prototype.Clone(forward<Args>(args)...);
            pool.push_back(static_cast<void*>(obj));
        }
    }

private:
    static unordered_map<type_index, vector<void*>> s_pools;
};

