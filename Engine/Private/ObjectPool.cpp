#include "ObjectPool.h"
#include "GameObject.h"
using namespace std;
template <> unordered_map<type_index, vector<void*>> ObjectPool<CGameObject>::s_pools;
template <> std::mutex ObjectPool<Engine::CGameObject>::m_mutex{};
template <> bool ObjectPool<Engine::CGameObject>::S_Clear = false;