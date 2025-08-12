#include "Base.h"

BEGIN(Engine)
class Collider_Manager final : public CBase
{
private:
    Collider_Manager();
    virtual ~Collider_Manager() = default;

public:
    HRESULT Initialize();


    HRESULT Add_Monster( class CGameObject* Monster);
    HRESULT Add_MonsterBullet( class CGameObject* MonsterBullet);
    HRESULT Add_Collider(_float Damage, class CCollider* Collider);
    HRESULT Add_Interctive(class CGameObject* Interctive);
    HRESULT Check_Collider_PlayerCollison();
    HRESULT Check_Inetrect_Player();
    void All_Collison_check();
    HRESULT Player_To_Monster_Ray_Collison_Check();
 
    HRESULT Player_To_Monster_Bullet_Collison();
    void Clear();
    HRESULT Find_Cell();
    HRESULT changeCellType(_int type);
    HRESULT Set_Collison(_bool SetColl) {
        m_bIsColl = SetColl;
        return S_OK;
    }

    _vector Get_RayPos()
    {
        return m_vRayPos;
    }

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    list <class CActor*> m_MonsterList;
    list <class CSkill*>m_MonsterBullet;
    list <class CCollider*>m_ColliderList;
    list<class CGameObject*> m_interctiveList;

    _float m_ColliderDamage{ 0.f };
    _uint   m_iLevel;
    _bool m_bIsColl = { false };
    _vector m_vRayPos{};

public:
    static Collider_Manager* Create();
    virtual void Free() override;
};
END
