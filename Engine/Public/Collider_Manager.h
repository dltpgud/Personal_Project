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
    HRESULT Check_Collider_PlayerCollison();
    void All_Collison_check();
    HRESULT Player_To_Monster_Collison_Check();
    HRESULT Player_To_Monster_Ray_Collison_Check();
 
    HRESULT Player_To_Monster_Bullet_Collison();
    void Clear();
    HRESULT Find_Cell();
    HRESULT Set_Collison(_bool SetColl) {
        m_bIsColl = SetColl;
        return S_OK;
    }



private:
    class CGameInstance* m_pGameInstance = { nullptr };
    list <class CActor*> m_MonsterList;
    list <class CSkill*>m_MonsterBullet;
    list <class CCollider*>m_ColliderList;
    

    _float m_ColliderDamage{ 0.f };
    _uint   m_iLevel;
    _bool m_bIsColl = { false };


public:
    static Collider_Manager* Create();
    virtual void Free() override;
};
END
