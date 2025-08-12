#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)
class CMecanoBot final : public CMonster
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
    CMecanoBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMecanoBot(const CMecanoBot& Prototype);
    virtual ~CMecanoBot() = default;

public:

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void HIt_Routine()override;
    virtual void Dead_Routine() override;


private:
    HRESULT Add_Components();
    HRESULT Add_PartObjects();

public:
    static CMecanoBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
    ///////////////////////////////////MONSTER_ANIM_DATA/////////////////////////////////////////
    // enum STATE
    // {
    //  ST_Idle,     0
    //  ST_JetPack,1
    //  ST_PreShoot,2
    //  ST_Run_Back,3
    //  ST_Run_Front,4
    //  ST_Run_Left,5
    //  ST_Run_Left_Back,6
    //  ST_Run_Right,7
    //  ST_Run_Right_Back,8
    //  ST_Shoot,9
    //  ST_Special_IdIe,10
    //  ST_Stagger_Back,11
    //  ST_Stagger_Front,12
    //  ST_Stagger_Left,13
    //  ST_Stagger_Right,14
    //  ST_Stun_Loop,15
    //  ST_Stun_Start,16
    // };