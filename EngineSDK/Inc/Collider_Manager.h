#include "Base.h"
#include "SpatialGrid.h"

BEGIN(Engine)
class Collider_Manager final : public CBase
{
public:
    enum CollGroup
    {
        COL_MONSTER,
        COL_MONSTER_SKILL,
        COL_INTERECT,
        COL_STATIC,
        COL_END
    };

private:
    Collider_Manager();
    virtual ~Collider_Manager() = default;

public:

    void All_Collison_check(_float fTimedelta);
    HRESULT Add_Collider(_int Damage, class CCollider* Collider);
    HRESULT Add_GameObject_To_ColGroup(class CGameObject* Obj, const _uint& Type);
    void Clear();

    HRESULT Find_Cell();
    HRESULT Set_Collison(_bool SetColl)
    {
        m_bIsColl = SetColl;
        return S_OK;
    }

private:
    HRESULT Initialize();
    HRESULT Check_Collider_PlayerCollison();
    HRESULT Check_Inetrect_Player();
    _bool Player_To_Monster_Ray_Collison_Check();
    HRESULT Monster_To_Monster_Collision();
    HRESULT Player_To_Monster_Bullet_Collison();
    HRESULT Player_To_Mash_Collison_for_Decal();
    HRESULT MonsterSkill_To_Mash_Collison(_float fTimedelta);


    struct LaserDecalCache
    {
        _vector vPrevPos = XMVectorZero();
        _vector vPrevDir = XMVectorZero();
        _vector vHitPos = XMVectorZero();
        _vector vHitNormal = XMVectorZero();
        bool bValid = false;
        float fTimeAcc = 0.f; // 주기적 갱신용
    };


private:
    class CGameInstance* m_pGameInstance = { nullptr };
    list <class CCollider*>m_ColliderList;
    list<class CGameObject*> m_GameObjeList[COL_END];
    
    _int m_ColliderDamage{ 0};
    _bool m_bIsColl = { false };
    _bool m_bStaticBuilt{false};
    unordered_map<class CComponent*, pair<_float3, _float3>> m_ModelCache;
    unordered_map<class CSkill*, LaserDecalCache> m_LaserCache;

private:
    CSpatialGrid m_SpatialGrid;
    _float m_lastCheckedTime = 0.0f;
    const _float m_checkInterval = 0.1f;
    _int iTime = 0;

public:
    static Collider_Manager* Create();
    virtual void Free() override;
};
END
