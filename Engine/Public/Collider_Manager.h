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
    HRESULT Add_Wall(_uint Ilevel, class CGameObject* wall);
    void All_Collison_check();
    HRESULT Player_To_Monster_Collison_Check();
    HRESULT Player_To_Monster_Ray_Collison_Check();
    void Clear(_uint Ilevel);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    list <class CActor*>* m_MonsterList;
    list <class CGameObject*>* m_Wall;
    _uint   m_iLevel;


public:
    static Collider_Manager* Create(_uint Ilevel);
    virtual void Free() override;
};
END
