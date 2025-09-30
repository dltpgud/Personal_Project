#pragma once
#include <typeindex>
#include <typeinfo>
#include "Engine_Defines.h"

template <typename Base> class ObjectPool
{
public:
    template <typename T, typename... Args> static T* Pop(T& prototype, Args&&... args)
    {
        auto& pool = s_pools[std::type_index(typeid(prototype))];

        if (!pool.empty())
        {
            void* raw = pool.back();
            pool.pop_back();

            T* obj = static_cast<T*>(raw);
            obj->Initialize(std::forward<Args>(args)...);
            return obj;
        }

        return prototype.Clone(std::forward<Args>(args)...);
    }

    template <typename T> static void Push(T* obj)
    {
        if (!obj)
            return;

        s_pools[std::type_index(typeid(*obj))].push_back(static_cast<void*>(obj));
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
            cout << pair.first.name() << endl;
        }
        s_pools.clear();
    }

private:
    static std::unordered_map<std::type_index, std::vector<void*>> s_pools;
};
