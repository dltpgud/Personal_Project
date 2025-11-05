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
    Collider_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Collider_Manager() = default;

public:

    void All_Collison_check(_float fTimedelta);
    HRESULT Add_Collider(_int Damage, class CCollider* Collider);
    HRESULT Add_GameObject_To_ColGroup(class CGameObject* Obj, const _uint& Type);
    void Clear();

    HRESULT Find_Cell(const _float2& vMin, const _float2& vMax, _float cellSize);
    HRESULT Set_Collison(_bool SetColl)
    {
        m_bIsColl = SetColl;
        return S_OK;
    }

    
#ifdef _DEBUG
    HRESULT Render();
#endif

private:
    HRESULT Initialize();
    HRESULT Check_Collider_PlayerCollison();
    HRESULT Check_Inetrect_Player();
    _bool Player_To_Monster_Ray_Collison_Check();
    HRESULT Monster_To_Monster_Collision();
    HRESULT Player_To_Monster_Bullet_Collison();
    HRESULT Player_To_Mash_Collison_for_Decal();
    HRESULT MonsterSkill_To_Mash_Collison(_float fTimedelta);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    list <class CCollider*>m_ColliderList;
    list<class CGameObject*> m_GameObjeList[COL_END];
    
    _int m_ColliderDamage{ 0};
    _bool m_bIsColl = { false };
    _bool m_bStaticBuilt{false};

private:
    CSpatialGrid m_SpatialGrid;


#ifdef _DEBUG
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};
    PrimitiveBatch<VertexPositionColor>* m_pBatch{};
    BasicEffect* m_pEffect{};
    ID3D11InputLayout* m_pInputLayout{};
#endif

public:
    static Collider_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};
END
