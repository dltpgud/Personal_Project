#include "Base.h"

BEGIN(Engine)
class Collider_Manager final : public CBase
{
public:
    enum CollGroup
    {
        COL_MONSTER,
        COL_MONSTER_SKILL,
        COL_INTERECT,
        COL_DECAL,
        COL_END
    };

private:
    Collider_Manager();
    virtual ~Collider_Manager() = default;

public:
    HRESULT Initialize();

    HRESULT Add_Collider(_int Damage, class CCollider* Collider);
    HRESULT Add_GameObject_To_ColGroup(class CGameObject* Obj, const _uint& Type);
    HRESULT Check_Collider_PlayerCollison();
    HRESULT Check_Inetrect_Player();
    void All_Collison_check();
    _bool Player_To_Monster_Ray_Collison_Check();
    HRESULT Monster_To_Monster_Collision();
    HRESULT Player_To_Monster_Bullet_Collison();
    HRESULT Player_To_Mash_Collison_for_Decal();
    HRESULT MonsterBullet_To_Mash_Collison_for_Decal();
    void Clear();
    HRESULT Find_Cell();
    HRESULT Set_Collison(_bool SetColl) {
        m_bIsColl = SetColl;
        return S_OK;
    }

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    list <class CCollider*>m_ColliderList;
    list<class CGameObject*> m_GameObjeList[COL_END];
    
    _int m_ColliderDamage{ 0};

    _bool m_bIsColl = { false };
 

public:
    static Collider_Manager* Create();
    virtual void Free() override;
};
END
