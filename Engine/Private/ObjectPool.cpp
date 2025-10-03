#include "ObjectPool.h"
#include "GameObject.h"
#include "Decal.h"
using namespace std;
template <> unordered_map<type_index, vector<void*>> ObjectPool<CGameObject>::s_pools;
template <> unordered_map<type_index, vector<void*>> ObjectPool<CDecal>::s_pools;
