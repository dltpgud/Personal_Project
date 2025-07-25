#pragma once
#include "Client_Defines.h"
#include "Actor.h"
BEGIN(Engine)

END

BEGIN(Client)
class CMonsterHP;
class CGunPawn final : public CActor
{
   
public:
    enum PARTOBJID
    {
        PART_BODY,
        PART_WEAPON,
        PART_EFFECT,
        PART_HP,
        PART_END
    };
    enum STATE
    {
        ST_GRENADE_PRESHOOT,
        ST_GRENADE_SHOOT,   
        ST_JETPACK,         
        ST_PRESHOOT,        
        ST_IDLE,            
        ST_SHOOT01,         
        ST_SHOOT03,         
        ST_STURN_LOOP,      
        ST_RUN_BACK,        
        ST_RUN_BACK_LEFT,  
        ST_RUN_BACK_RIGHT,  
        ST_RUN_BACK_FRONT,  
        ST_RUN_LEFT,        
        ST_RUN_RIGHT,       
        ST_STAGGER_BACK,    
        ST_STAGGER_FRONT, 
        ST_STAGGER_LEFT,    
        ST_STAGGER_RIGHT, 
        ST_STUN_START, 
        ST_HIT_BACK,   
        ST_HIT_FRONT,
        ST_HIT_LEFT,  
        ST_HIT_RIGHT 
    };                   

private:
    CGunPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGunPawn(const CGunPawn& Prototype);
    virtual ~CGunPawn() = default;

public:
    /* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
    virtual HRESULT Initialize_Prototype() override;

    /* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
    /* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void HIt_Routine()override;
    virtual void Dead_Routine(_float fTimeDelta) override;


    void NON_intersect(_float fTimedelta);

private:
    HRESULT Add_Components();
    HRESULT Add_PartObjects();
    HRESULT Bind_ShaderResources();
    CMonsterHP* m_pPartHP = { nullptr };
    _float m_FireTime{ 0.f };


public:
    static CGunPawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
