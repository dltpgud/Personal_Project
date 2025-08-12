#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CGunPawn final : public CMonster
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
    CGunPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGunPawn(const CGunPawn& Prototype);
    virtual ~CGunPawn() = default;

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
    static CGunPawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END

///////////////////////////////////MONSTER_ANIM_DATA/////////////////////////////////////////
// enum STATE
//{
//     ST_GRENADE_PRESHOOT, 0
//     ST_GRENADE_SHOOT,   1
//     ST_JETPACK,         2
//     ST_PRESHOOT,        3 //¸Á°¡Áü
//     ST_IDLE,            4
//     ST_SHOOT01,         5  //¸Á°¡Áü
//     ST_SHOOT03,         6 //¸Á°¡Áü
//     ST_STURN_LOOP,      7
//     ST_RUN_BACK,        8
//     ST_RUN_BACK_LEFT,  9
//     ST_RUN_BACK_RIGHT,  10
//     ST_RUN_BACK_FRONT,  11
//     ST_RUN_LEFT,        12
//     ST_RUN_RIGHT,       13
//     ST_STAGGER_BACK,    14
//     ST_STAGGER_FRONT,   15
//     ST_STAGGER_LEFT,    16
//     ST_STAGGER_RIGHT, 17
//     ST_STUN_START, 18
//     ST_HIT_BACK,   19 ¸Á°¡Áø ¾Ö´Ô
//     ST_HIT_FRONT,  20
//     ST_HIT_LEFT,  21
//     ST_HIT_RIGHT  22
// };