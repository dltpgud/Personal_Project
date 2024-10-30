#include "Base.h"

BEGIN(Engine)
class Collider_Manager final : public CBase
{
private:
    Collider_Manager();
    virtual ~Collider_Manager() = default;

public:
    HRESULT Initialize(_uint Ilevel);


    HRESULT Add_Monster(_uint Ilevel, class CGameObject* Monster);
    void All_Collison_check();
    HRESULT Player_To_Monster_Collison_Check();
    HRESULT Player_To_Monster_Ray_Collison_Check();
    void Clear(_uint Ilevel);
    HRESULT Find_Cell(_uint Ilevel);
    HRESULT Set_Collison(_bool SetColl) {
        m_bIsColl = SetColl;
        return S_OK;
    }



private:
    class CGameInstance* m_pGameInstance = { nullptr };
    list <class CActor*>* m_MonsterList;
    _uint   m_iLevel;
    _bool m_bIsColl = { false };


public:
    static Collider_Manager* Create(_uint Ilevel);
    virtual void Free() override;
};
END
