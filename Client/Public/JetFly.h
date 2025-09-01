#pragma once
#include "Client_Defines.h"
#include "Monster.h"
BEGIN(Engine)

END

BEGIN(Client)
class CJetFly final : public CMonster
{
public:

    enum STATE : _ubyte
    {
        ST_IDLE,
        ST_SHOOT,
        ST_HIT,
        ST_DEAD,
        ST_MOVE,
        ST_END
    }; 

private:
    CJetFly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CJetFly(const CJetFly& Prototype);
    virtual ~CJetFly() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void HIt_Routine()override;
    virtual void Dead_Routine() override;
    virtual void Wake_up() override;
    virtual void Seeping() override;

private:
    HRESULT Add_Components();
    HRESULT Add_PartObjects();

public:
    static CJetFly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END

/////////////////////////////MODEL_ANIM_DATA///////////////////////////////////////////
// enum STATE
//{
//  ST_BarrelRoll_Left, 0
//  ST_BarrelRoll_Right, 1
//  ST_Idle, 2
//  ST_Shoot, 3
//  ST_Sragger, 4
//  ST_Walk_Back, 5
//  ST_Walk_Front, 6
//  ST_Walk_Left, 7
//  ST_Walk_Right, 8
//  ST_Hit_Back, 9     // ´Ù ¸Á°¡Áü
//  ST_Hit_Front, 10
//  ST_Hit_Left, 11
//  ST_Hit_Right 12
// };
