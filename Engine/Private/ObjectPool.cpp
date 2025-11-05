#include "ObjectPool.h"
#include "GameObject.h"
using namespace std;
template <> unordered_map<type_index, vector<void*>> ObjectPool<CGameObject>::s_pools;

