#pragma once
#include "Client_Defines.h"
#include "Actor.h"
BEGIN(Engine)
class CActor;
class CGameObject;
END

BEGIN(Client)
class CMonster  : public CActor
{
public:
    enum PARTOBJID
    {
        PART_BODY,
        PART_HP,
        PART_END
    };

    enum ATTCAK_STATE 
    {
        AT_FRONT,
        AT_BACK,
        AT_LEFT,
        AT_RIGHT,
    };

    enum FLAG
    {
        FLAG_DEAD,
        FLAG_END
    };
    
protected:
    CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMonster(const CMonster& Prototype);
    virtual ~CMonster() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void Wake_up() {};
    virtual void Seeping() {};

    void Compute_Length();
    void Compute_Angle();
    _bool IsLookAtPlayer(_float angle);

    _float* Get_fAttackLength() { return &m_fAttackLength;}
    _int* Get_AttackAngle() { return &m_iAttackAngleType;}
   

protected:
    _float m_fAttackLength{};
    _int m_iAttackAngleType{};


public:
    static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
