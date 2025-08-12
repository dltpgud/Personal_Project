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
        PART_EFFECT,
        PART_END
    };

    enum ATTCAK_STATE 
    {
        AT_FRONT,
        AT_BACK,
        AT_LEFT,
        AT_RIGHT,
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

    void Compute_Length();
    void Compute_Angle();
    _float* Get_fLength()  {
        return &m_fLength;
    }

    _int* Get_AttackAngle()
    {
        return &m_iAttackAngleType;
    }

protected:
    _float m_fLength{};
    _int m_iAttackAngleType{};

public:
    static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
